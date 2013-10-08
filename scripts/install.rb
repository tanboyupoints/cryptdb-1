#!/usr/bin/env ruby

require 'etc'
require 'fileutils'

$usage =
    "Usage: ./install <path-to-cryptdb> [automake-version] [gcc-version]"

SHADOW_PATH = "/var/lib/shadow-mysql"
PROXY_NAME = "proxy-src"
MYSQL_NAME = "mysql-src"
TAR_GZ = ".tar.gz"

class String
    def cyan
        "\033[36m#{self}\033[0m"
    end

    def bold
        "\033[1m#{self}\033[22m"
    end
end

def get_pkgs
    p_puts "Retrieving packages..."

    pkg_shell = ShellDoer.new("~")
    pkg_shell.pretty_execute(%q{
        sudo apt-get install liblua5.1-0-dev libntl-dev                \
                             libmysqlclient-dev libssl-dev libbsd-dev  \
                             libevent-dev libglib2.0-dev libgmp-dev    \
                             mysql-server libaio-dev automake          \
                             gtk-doc-tools flex cmake libncurses5-dev  \
                             bison g++ make                            \
        1>&2
    })
end

def fn(cdb_path, in_make_v=nil, in_gcc_v=nil)
    cryptdb_path = File.expand_path(cdb_path)
    cryptdb_shell = ShellDoer.new(cryptdb_path)
    bins_path = File.join(cryptdb_path, "bins/")

    #############################
    #        mysql-proxy
    # ###########################
    # + automake fixups.
    p_puts "Checking automake..."

    automake_version = 
        if in_make_v
            in_make_v
        else
            first_line_version(%x(automake --version))
        end

    if automake_version.nil?
        fail no_version_fail("automake")
    end

    p_puts "Building mysql-proxy..."

    # untar
    proxy_path = File.join(cryptdb_path, PROXY_NAME)
    proxy_tar_path = File.join(bins_path, PROXY_NAME) + TAR_GZ
    cryptdb_shell.pretty_execute("tar zxf #{proxy_tar_path}") 

    mp_shell = ShellDoer.new(proxy_path)
    config_path = File.join(proxy_path, "configure.in")
    if version_gte?(automake_version, "1.12") 
        big = File.join(proxy_path, "big_configure.in")
        FileUtils.copy(big, config_path)
    else
        little = File.join(proxy_path, "little_configure.in")
        FileUtils.copy(little, config_path)
    end

    mp_shell.pretty_execute("./autogen.sh")
    mp_shell.pretty_execute("./configure --enable-maintainer-mode --with-lua=lua5.1")
    mp_shell.pretty_execute("make")
    mp_shell.pretty_execute("sudo make install")
    File.delete(config_path)
    mp_shell.pretty_execute("rm -rf #{proxy_path}")

    #############################
    #            gcc
    #############################
    p_puts "Checking gcc..."

    gcc_version =
        if in_gcc_v
            in_gcc_v
        else
            first_line_version(%x(gcc --version))
        end

    if gcc_version.nil?
        fail no_version_fail("gcc")
    end

    if !version_gte?(gcc_version, "4.6")
        fail("update your gcc version to >= 4.6 before installing!")
    end


    #############################
    #           mysql
    #############################
    p_puts "Building mysql..."

    # untar
    mysql_path = File.join(cryptdb_path, MYSQL_NAME)
    mysql_tar_path = File.join(bins_path, MYSQL_NAME) + TAR_GZ
    cryptdb_shell.pretty_execute("tar zxf #{mysql_tar_path}")

    mysql_build_path = File.join(mysql_path, "/build")
    Dir.mkdir(mysql_build_path) if false == File.exists?(mysql_build_path)

    mysql_shell = ShellDoer.new(mysql_build_path)
    mysql_shell.pretty_execute("cmake -DWITH_EMBEDDED_SERVER=on ..")
    mysql_shell.pretty_execute("make")

    #############################
    #          cryptdb
    #############################
    p_puts "Building cryptdb..."

    cryptdb_shell.pretty_execute("make MYSRC=#{mysql_path}")
    cryptdb_shell.pretty_execute("sudo make install MYSRC=#{mysql_path}")
    cryptdb_shell.pretty_execute("sudo rm -rf #{SHADOW_PATH}")
    cryptdb_shell.pretty_execute("sudo mkdir #{SHADOW_PATH}")
    cryptdb_shell.pretty_execute("sudo chown #{Etc.getlogin} #{SHADOW_PATH}")
end

class ShellDoer
    def initialize(dir)
        @dir = dir
    end

    def pretty_execute(cmd)
        %x(cd #{@dir} && #{cmd} 1>&2)
        if $?.exitstatus != 0
            fail "`#{cmd}` failed"
        end
    end
end

def no_version_fail(name)
    "unable to determine #{name} version, supply version # thru command line argument.\n#{$usage}\n"
end

def first_line_version(text)
    /([0-9]+\.[0-9]+\.?[0-9]*)/.match(text.split("\n").first)[0]
end

def parse_version(v)
    data = /([0-9]+)\.([0-9]+)\.?([0-9]*)/.match(v)
    if data.nil?
        return nil
    else
        return [data[1].to_i, data[2].to_i,
                data[3].empty? ? 0 : data[3].to_i]
    end
end

# is 'version_a' >= 'version_b'
def version_gte?(v_a, v_b)
    parse_version(v_a).zip(parse_version(v_b)).inject(0) do |acc, (a, b)|
        if a > b then
            break true
        elsif a < b then
            break false
        else
            true
        end
    end
end

def p_puts(output_me)
    puts output_me.cyan.bold
end

#############################
#############################
#   Execution Begins Here
#############################
#############################
if ARGV.size() < 1 || ARGV.size() > 3
    fail $usage
end

get_pkgs()
fn(ARGV[0], ARGV[1], ARGV[2])


