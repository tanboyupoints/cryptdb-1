<?php
/*
 PHP Mini MySQL Admin
 (c) 2004-2012 Oleg Savchuk <osalabs@gmail.com> http://osalabs.com

 Light standalone PHP script for quick and easy access MySQL databases.
 http://phpminiadmin.sourceforge.net

 Dual licensed: GPL v2 and MIT, see texts at http://opensource.org/licenses/
*/

 $ACCESS_PWD=''; #!!!IMPORTANT!!! this is script access password, SET IT if you want to protect you DB from public access

 #DEFAULT db connection settings
 # --- WARNING! --- if you set defaults - it's recommended to set $ACCESS_PWD to protect your db!
$DBDEF=array(
    'user'=>"",#required
    'pwd'=>"", #required
    'db'=>"",  #optional, default DB
    'host'=>"",#optional
    'port'=>"",#optional
    'chset'=>"utf8",#optional, default charset
);

$CRYPTDBDEF=array(
    'user'=>"",#required
    'pwd'=>"", #required
    'db'=>"",  #optional, default DB
    'host'=>"",#optional
    'port'=>"",#optional
    'chset'=>"utf8",#optional, default charset
);

 date_default_timezone_set('UTC');#required by PHP 5.1+

//constants
 $VERSION='1.8.120510';
 $MAX_ROWS_PER_PAGE=50; #max number of rows in select per one page
 $D="\r\n"; #default delimiter for export
 $BOM=chr(239).chr(187).chr(191);
 $SHOW_T="SHOW TABLE STATUS";
 $DB=array(); #working copy for DB settings
 $CRYPTDB=array(); #working copy for CryptDB settings


 $self=$_SERVER['PHP_SELF'];

 session_start();
 if (!isset($_SESSION['XSS'])) $_SESSION['XSS']=get_rand_str(16);
 $xurl='XSS='.$_SESSION['XSS'];

 ini_set('display_errors',1);  #TODO turn off before deploy
 error_reporting(E_ALL ^ E_NOTICE);

//strip quotes if they set
 if (get_magic_quotes_gpc()){
     $_COOKIE=array_map('killmq',$_COOKIE);
     $_REQUEST=array_map('killmq',$_REQUEST);
 }

 if (!$ACCESS_PWD) {
     $_SESSION['is_logged']=true;
     loadcfg();
 }

 if ($_REQUEST['login']){
     if ($_REQUEST['pwd']!=$ACCESS_PWD){
         $err_msg="Invalid password. Try again";
     }else{
         $_SESSION['is_logged']=true;
         loadcfg();
     }
 }

 if ($_REQUEST['logoff']){
     check_xss();
     $_SESSION = array();
     savecfg();
     session_destroy();
     $url=$self;
     if (!$ACCESS_PWD) $url='/';
     header("location: $url");
     exit;
 }

 if (!$_SESSION['is_logged']){
     print_login();
     exit;
 }

 if ($_REQUEST['savecfg']){
     check_xss();
     savecfg();
 }

 loadsess();

 if ($_REQUEST['showcfg']){
     print_cfg();
     exit;
 }

//Connection end


 //get initial values
 $SQLq=trim($_REQUEST['q']);
 $page=$_REQUEST['p']+0;
 if ($_REQUEST['refresh'] && $DB['db'] && preg_match('/^show/',$SQLq) ) $SQLq=$SHOW_T;
 
  cryptdb_connect('nodie');


 /*
  * Commands handler
  */
 if (db_connect('nodie')){
     $time_start=microtime_float();

     //echo "->$" . "." . $DB['db'] . "." . $_POST['describe_table'] . "." . "$<-"; 

     if ($_REQUEST['phpinfo']){
         ob_start();phpinfo();$sqldr='<div style="font-size:130%">'.ob_get_clean().'</div>';
     }else{
         if ($DB['db']){

             /*
              * Commands:
              * describe_table
              *
              *
              */
             if($_POST['cryptdb_describe_table'])
             {
                 $q = "describe " . $DB['db'] . "." . $_POST['cryptdb_describe_table'];
                 do_sql($q);

                 //TODO: handle all cryptdb web commands
             } else {
                 if ($_REQUEST['shex']){
                     print_export();
                 }elseif (!$_REQUEST['refresh'] || preg_match('/^select|show|explain|desc/i',$SQLq) ){
                     if ($SQLq)check_xss();
                     do_sql($SQLq);#perform non-select SQL only if not refresh (to avoid dangerous delete/drop)
                 }
             }
         }else{
             if ( $_REQUEST['refresh'] ){
                 check_xss();do_sql('show databases');
             }elseif ( preg_match('/^show\s+(?:databases|status|variables|process)/i',$SQLq) ){
                 check_xss();do_sql($SQLq);
             }else{
                 $err_msg="Select Database first";
                 if (!$SQLq) do_sql("show databases");
             }
         }
     }
     $time_all=ceil((microtime_float()-$time_start)*10000)/10000;

     print_screen();
 }else{
    print_cfg();
 }

function do_sql($q){
 global $dbh,$last_sth,$last_sql,$reccount,$out_message,$SQLq,$SHOW_T;
 $SQLq=$q;

 if (!do_multi_sql($q)){
     $out_message="Error: ".mysql_error($dbh);
 }else{
     if ($last_sth && $last_sql){
         $SQLq=$last_sql;
         if (preg_match("/^select|show|explain|desc/i",$last_sql)) {
             if ($q!=$last_sql) $out_message="Results of the last select displayed:";
             display_select($last_sth,$last_sql);
         } else {
             $reccount=mysql_affected_rows($dbh);
             $out_message="Done.";
             if (preg_match("/^insert|replace/i",$last_sql)) $out_message.=" Last inserted id=".get_identity();
             if (preg_match("/^drop|truncate/i",$last_sql)) do_sql($SHOW_T);
         }
     }
 }
}

function do_cryptdb_sql($q){
 global $cryptdbh,$last_sth,$last_sql,$reccount,$out_message,$SQLq,$SHOW_T;
 $SQLq=$q;

 if (!do_multi_sql($q)){
     $out_message="Error: ".mysql_error($cryptdbh);
 }else{
     if ($last_sth && $last_sql){
         $SQLq=$last_sql;
         if (preg_match("/^select|show|explain|desc/i",$last_sql)) {
             if ($q!=$last_sql) $out_message="Results of the last select displayed:";
             display_select($last_sth,$last_sql);
         } else {
             $reccount=mysql_affected_rows($cryptdbh);
             $out_message="Done.";
             if (preg_match("/^insert|replace/i",$last_sql)) $out_message.=" Last inserted id=".get_identity();
             if (preg_match("/^drop|truncate/i",$last_sql)) do_sql($SHOW_T);
         }
     }
 }
}


function display_select($sth,$q){
    //echo $q;
    global $dbh,$DB,$sqldr,$reccount,$is_sht,$xurl;
    $rc=array("o","e");
    $dbn=$DB['db'];
    $sqldr='';

    $is_shd=(preg_match('/^show\s+databases/i',$q));
    $is_sht=(preg_match('/^show\s+tables|^SHOW\s+TABLE\s+STATUS/',$q));
    $is_show_crt=(preg_match('/^show\s+create\s+table/i',$q));

    if ($sth===FALSE or $sth===TRUE) return;#check if $sth is not a mysql resource

    $reccount=mysql_num_rows($sth);
    $fields_num=mysql_num_fields($sth);

    $w='';
    if ($is_sht || $is_shd) {
        $w='wa';
        $url='?'.$xurl."&db=$dbn";
        $sqldr.="<div class='dot'>
            &nbsp;MySQL Server:
            &nbsp;&#183;<a href='$url&q=show+variables'>Show Configuration Variables</a>
            &nbsp;&#183;<a href='$url&q=show+status'>Show Statistics</a>
            &nbsp;&#183;<a href='$url&q=show+processlist'>Show Processlist</a>
            <br>";
        if ($is_sht) $sqldr.="&nbsp;Database:&nbsp;&#183;<a href='$url&q=show+table+status'>Show Table Status</a>";
        $sqldr.="</div>";
    }
    if ($is_sht){
        $abtn="&nbsp;<input type='submit' value='Export' onclick=\"sht('exp')\">
            <input type='submit' value='Drop' onclick=\"if(ays()){sht('drop')}else{return false}\">
            <input type='submit' value='Truncate' onclick=\"if(ays()){sht('trunc')}else{return false}\">
            <input type='submit' value='Optimize' onclick=\"sht('opt')\">
            <b>selected tables</b>";
        $sqldr.=$abtn."<input type='hidden' name='dosht' value=''>";
    }

    $sqldr.="<table class='res $w'>";
    $headers="<tr class='h'>";
    if ($is_sht) $headers.="<td><input type='checkbox' name='cball' value='' onclick='chkall(this)'></td>";
    for($i=0;$i<$fields_num;$i++){
        if ($is_sht && $i>0) break;
        $meta=mysql_fetch_field($sth,$i);
        $headers.="<th>".$meta->name."</th>";
    }
    if ($is_shd) $headers.="<th>show create database</th><th>show table status</th><th>show triggers</th>";
    if ($is_sht) $headers.="<th>engine</th><th>~rows</th><th>data size</th><th>index size</th><th>show create table</th><th>explain</th><th>indexes</th><th>export</th><th>drop</th><th>truncate</th><th>optimize</th><th>repair</th>";
    $headers.="</tr>\n";
    $sqldr.=$headers;
    $swapper=false;
    while($row=mysql_fetch_row($sth)){
        $sqldr.="<tr class='".$rc[$swp=!$swp]."' onmouseover='tmv(this)' onmouseout='tmo(this)' onclick='tc(this)'>";
        for($i=0;$i<$fields_num;$i++){
            $v=$row[$i];$more='';
            if ($is_sht && $v){
                if ($i>0) break;
                $vq='`'.$v.'`';
                $url='?'.$xurl."&db=$dbn";
                $v="<input type='checkbox' name='cb[]' value=\"$vq\"></td>"
                    ."<td><a href=\"$url&q=select+*+from+$vq\">$v</a></td>"
                    ."<td>".$row[1]."</td>"
                    ."<td align='right'>".$row[4]."</td>"
                    ."<td align='right'>".$row[6]."</td>"
                    ."<td align='right'>".$row[8]."</td>"
                    ."<td>&#183;<a href=\"$url&q=show+create+table+$vq\">sct</a></td>"
                    ."<td>&#183;<a href=\"$url&q=explain+$vq\">exp</a></td>"
                    ."<td>&#183;<a href=\"$url&q=show+index+from+$vq\">ind</a></td>"
                    ."<td>&#183;<a href=\"$url&shex=1&t=$vq\">export</a></td>"
                    ."<td>&#183;<a href=\"$url&q=drop+table+$vq\" onclick='return ays()'>dr</a></td>"
                    ."<td>&#183;<a href=\"$url&q=truncate+table+$vq\" onclick='return ays()'>tr</a></td>"
                    ."<td>&#183;<a href=\"$url&q=optimize+table+$vq\" onclick='return ays()'>opt</a></td>"
                    ."<td>&#183;<a href=\"$url&q=repair+table+$vq\" onclick='return ays()'>rpr</a>";
            }elseif ($is_shd && $i==0 && $v){
                $url='?'.$xurl."&db=$v";
                $v="<a href=\"$url&q=SHOW+TABLE+STATUS\">$v</a></td>"
                    ."<td><a href=\"$url&q=show+create+database+`$v`\">sct</a></td>"
                    ."<td><a href=\"$url&q=show+table+status\">status</a></td>"
                    ."<td><a href=\"$url&q=show+triggers\">trig</a></td>"
                    ;
            }else{
                if (is_null($v)) $v="NULL";
                $v=htmlspecialchars($v);
            }
            if ($is_show_crt) $v="<pre>$v</pre>";
            $sqldr.="<td>$v".(!strlen($v)?"<br>":'')."</td>";

        }
            if($_POST['cryptdb_describe_table'])
            {
                $sqldr.= "<td><select name=\"teste\" id=\"xxx\"> 
                    <option selected>Sensitive Field</option> 
                    <option>Best Effort Encryption</option> 
                    <option>Unencrypted</option>
                    </select></td>";
                $sqldr.= "<td><form action=\"\" method=\"post\">
                    <button name=\"foo\" value=\"upvote\">Execute</button>
                    </form></td>";
            }
            $sqldr.= "</td></tr>\n";
    }
    $sqldr.="</table>\n".$abtn;

}

function print_header(){
 global $err_msg,$VERSION,$DB,$CRYPTDB,$dbh,$self,$is_sht,$xurl,$SHOW_T;
 $dbn=$DB['db'];
?>

<!-- HTML BODY -->


<!DOCTYPE html>
<html>
<head><title>CryptDB</title>
<meta charset="utf-8">


<!-- CSS STYLE DEFINITIONS -->
<style type="text/css">
body{font-family:Arial,sans-serif;font-size:80%;padding:2;margin:2}
th,td{padding:0;margin:0}
div{padding:3px}
pre{font-size:50%}
.nav{text-align:center}
.ft{text-align:right;margin-top:10px;font-size:smaller}
.inv{background-color:#069;color:#FFF}
.inv a{color:#FFF}
table.res{width:50%;border-collapse:collapse;}
table.wa{width:auto}
table.res th,table.res td{padding:2px;border:1px solid #fff}
table.restr{vertical-align:top}
tr.e{background-color:#CCC}
tr.o{background-color:#EEE}
tr.h{background-color:#99C}
tr.s{background-color:#FF9}
.err{color:#F33;font-weight:bold;text-align:center}
.frm{width:400px;border:1px solid #999;background-color:#eee;text-align:left}
.frm label.l{width:100px;float:left}
.dot{border-bottom:1px dotted #000}
.ajax{text-decoration: none;border-bottom: 1px dashed;}
.qnav{width:30px}
</style>

<script type="text/javascript">
var LSK='pma_',LSKX=LSK+'max',LSKM=LSK+'min',qcur=0,LSMAX=32;

function $(i){return document.getElementById(i)}
function frefresh(){
    var F=document.DF;
    F.method='get';
    F.refresh.value="1";
    F.submit();
}
function go(p,sql){
    var F=document.DF;
    F.p.value=p;
    if(sql)F.q.value=sql;
    F.submit();
}
function ays(){
    return confirm('Are you sure to continue?');
}
function chksql(){
    var F=document.DF,v=F.q.value;
    if(/^\s*(?:delete|drop|truncate|alter)/.test(v)) if(!ays())return false;
    if(lschk(1)){
        var lsm=lsmax()+1,ls=localStorage;
        ls[LSK+lsm]=v;
        ls[LSKX]=lsm;
        //keep just last LSMAX queries in log
        if(!ls[LSKM])ls[LSKM]=1;
        var lsmin=parseInt(ls[LSKM]);
        if((lsm-lsmin+1)>LSMAX){
            lsclean(lsmin,lsm-LSMAX);
        }
    }
    return true;
}
function tmv(tr){
    tr.sc=tr.className;
    tr.className='h';
}
function tmo(tr){
    tr.className=tr.sc;
}
function tc(tr){
    tr.className='s';
    tr.sc='s';
}
function lschk(skip){
    if (!localStorage || !skip && !localStorage[LSKX]) return false;
    return true;
}
function lsmax(){
    var ls=localStorage;
    if(!lschk() || !ls[LSKX])return 0;
    return parseInt(ls[LSKX]);
}
function lsclean(from,to){
    ls=localStorage;
    for(var i=from;i<=to;i++){
        delete ls[LSK+i];ls[LSKM]=i+1;
    }
}
function q_prev(){
    var ls=localStorage;
    if(!lschk())return;
    qcur--;
    var x=parseInt(ls[LSKM]);
    if(qcur<x)qcur=x;
    $('q').value=ls[LSK+qcur];
}
function q_next(){
    var ls=localStorage;
    if(!lschk())return;
    qcur++;
    var x=parseInt(ls[LSKX]);
    if(qcur>x)qcur=x;
    $('q').value=ls[LSK+qcur];
}
function after_load(){
    qcur=lsmax();
}
function logoff(){
    if(lschk()){
        var ls=localStorage;
        var from=parseInt(ls[LSKM]),to=parseInt(ls[LSKX]);
        for(var i=from;i<=to;i++){
            delete ls[LSK+i];
        }
        delete ls[LSKM];delete ls[LSKX];
    }
}
function cfg_toggle(){
    var e=$('cfg-adv');
    e.style.display=e.style.display=='none'?'':'none';
}
<?php if($is_sht){?>
function chkall(cab){
    var e=document.DF.elements;
    if (e!=null){
        var cl=e.length;
        for (i=0;i<cl;i++){
            var m=e[i];if(m.checked!=null && m.type=="checkbox"){
                m.checked=cab.checked
            }
        }
    }
}
function sht(f){
    document.DF.dosht.value=f;
}


<?php }?>
</script>

</head>


<body onload="after_load()">
<form method="post" name="DF" action="<?php echo $self?>" enctype="multipart/form-data">
<input type="hidden" name="XSS" value="<?php echo $_SESSION['XSS']?>">
<input type="hidden" name="refresh" value="">
<input type="hidden" name="p" value="">


<!-- HEADER MENU -->
<div class="inv">
<?php if ($_SESSION['is_logged'] && $dbh){ ?>
 <a href="?<?php echo $xurl?>&q=show+databases">Databases</a>: 

<select name="db" onChange="frefresh()">
<?php echo get_db_select($dbn)?>
</select>

<?php if($dbn){ $z=" &#183; <a href='$self?$xurl&db=$dbn"; ?>
<?php if($_POST['cryptdb_describe_table'] && $dbn) { 
    $z.'&q='.urlencode($var);
} ?>
Tables:

<form action="'<?php echo $self?>'" method="post">
<select name="cryptdb_describe_table" onChange="location.href='<?php $var = "describe " . $_POST['cryptdb_describe_table']; echo $z.'&q='.urlencode($var)?>'">
<?php 
 $a=0;
 $array = get_db_tables($dbn) ; foreach($array as $key=>$value) 
{ 
     $accesskey = "Tables_in_" . $dbn;
?>
<option value="<?php echo $value[$accesskey]; ?>"><?php echo $value[$accesskey]; ?></option>
<?php } ?>

</select>
<input type="submit" value="Describe table">
</form>
<?php } ?>
<?php } ?>




<?php if ($GLOBALS['ACCESS_PWD']){?> | <a href="?<?php echo $xurl?>&logoff=1" onclick="logoff()">Logoff</a> <?php }?>
</div>
<div class="err"><?php echo $err_msg?></div>

<?php
}

function print_screen(){
    global $out_message, $SQLq, $err_msg, $reccount, $time_all, $sqldr, $page, $MAX_ROWS_PER_PAGE, $is_limited_sql;

    $nav='';
    if ($is_limited_sql && ($page || $reccount>=$MAX_ROWS_PER_PAGE) ){
        $nav="<div class='nav'>".get_nav($page, 10000, $MAX_ROWS_PER_PAGE, "javascript:go(%p%)")."</div>";
    }

    print_header();
?>

<textarea readonly id="q" name="q" cols="70" rows="5" style="width:50%"><?php echo $SQLq?></textarea><br>

Records: <b><?php echo $reccount?></b> in <b><?php echo $time_all?></b> sec<br>
<b><?php echo $out_message?></b>
<div class="sqldr">
<?php echo $nav.$sqldr.$nav; ?>
</div>
<?php
    print_footer();
}

function print_footer(){
?>
</form>
</body></html>
<?php
}

function print_login(){
    print_header();
?>
<center>
<h3>Access protected by password</h3>
<div style="width:400px;border:1px solid #999999;background-color:#eeeeee">
Password: <input type="password" name="pwd" value="">
<input type="hidden" name="login" value="1">
<input type="submit" value=" Login ">
</div>
</center>
<?php
    print_footer();
}


function print_cfg(){
    global $DB,$CRYPTDB,$err_msg,$self;
    print_header();
?>
<center>
<h3>CryptDB</h3>
<div class="frm">

<label class="l">DB user:</label><input type="text" name="v[user]" value="<?php echo $DB['user']?>"><br>
<label class="l">Password:</label><input type="password" name="v[pwd]" value=""><br>
<br>

<label class="l">CryptDB user:</label><input type="text" name="c[user]" value="<?php echo $CRYPTDB['user']?>"><br>
<label class="l">Password:</label><input type="password" name="c[pwd]" value=""><br>
<label class="l">Port:</label><input type="text" name="c[port]" value="<?php echo $CRYPTDB['port']?>" size="4"><br>

<div id="cfg-adv" style="display:none;">



<label class="l">Charset:</label>

<select name="v[chset]"><option value="">- default -</option><?php echo chset_select($DB['chset'])?>
</select><br>

<br>


<br><input type="checkbox" name="rmb" value="1" checked> Remember in cookies for 30 days
</div>
<center>
<input type="hidden" name="savecfg" value="1">
<input type="submit" value=" Apply "><input type="button" value=" Cancel " onclick="window.location='<?php echo $self?>'">
</center>
</div>
</center>
<?php
    print_footer();
}


//* utilities
function db_connect($nodie=0){
    global $dbh,$DB,$err_msg;

    $dbh=@mysql_connect($DB['host'].($DB['port']?":$DB[port]":''),$DB['user'],$DB['pwd']);
    if (!$dbh) {
        $err_msg='Cannot connect to the database because: '.mysql_error();
        if (!$nodie) die($err_msg);
    }

    if ($dbh && $DB['db']) {
        $res=mysql_select_db($DB['db'], $dbh);
        if (!$res) {
            $err_msg='Cannot select db because: '.mysql_error();
            if (!$nodie) die($err_msg);
        }else{
            if ($DB['chset']) db_query("SET NAMES ".$DB['chset']);
        }
    }

    return $dbh;
}

function cryptdb_connect($nodie=0){
    global $cryptdbh,$CRYPTDB,$err_msg;

    //TODO: Test this implementation
    return;


    $cryptdbh=@mysql_connect($CRYPTDB['host'].($CRYPTDB['port']?":$CRYPTDB[port]":''),$CRYPTDB['user'],$CRYPTDB['pwd']);
    if (!$cryptdbh) 
    {
        $err_msg='Cannot connect to the CryptDB proxy database because: '.mysql_error();
        echo $err_msg;
        //TODO:FIX this
        die($err_msg);
    }

    if ($cryptdbh && $CRYPTDB['db']) {
        $res=mysql_select_db($CRYPTDB['db'], $cryptdbh);
        if (!$res) {
            $err_msg='Cannot select cryptdb because: '.mysql_error();
            if (!$nodie) 
            {
                die($err_msg);
            }
        }else{
            if ($CRYPTDB['chset']) db_query("SET NAMES ".$CRYPTDB['chset']);
        }
    }

    return $cryptdbh;
}

function db_checkconnect($dbh1=NULL, $skiperr=0){
    global $dbh;
    if (!$dbh1) $dbh1=&$dbh;
    if (!$dbh1 or !mysql_ping($dbh1)) {
        db_connect($skiperr);
        $dbh1=&$dbh;
    }
    return $dbh1;
}

function cryptdb_checkconnect($cryptdbh1=NULL, $skiperr=0){
    global $cryptdbh;
    if (!$cryptdbh1) $cryptdbh1=&$cryptdbh;
    if (!$cryptdbh1 or !mysql_ping($cryptdbh1)) {
        cryptdb_connect($skiperr);
        $cryptdbh1=&$cryptdbh;
    }
    return $cryptdbh1;
}

function db_disconnect(){
    global $dbh;
    mysql_close($dbh);
}

function dbq($s){
    global $dbh;
    if (is_null($s)) return "NULL";
    return "'".mysql_real_escape_string($s,$dbh)."'";
}

function db_query($sql, $dbh1=NULL, $skiperr=0){
    $dbh1=db_checkconnect($dbh1, $skiperr);
    $sth=@mysql_query($sql, $dbh1);
    if (!$sth && $skiperr) return;
    if (!$sth) die("Error in DB operation:<br>\n".mysql_error($dbh1)."<br>\n$sql");
    return $sth;
}

function db_array($sql, $dbh1=NULL, $skiperr=0, $isnum=0){#array of rows
    $sth=db_query($sql, $dbh1, $skiperr);
    if (!$sth) return;
    $res=array();
    if ($isnum){
        while($row=mysql_fetch_row($sth)) $res[]=$row;
    }else{
        while($row=mysql_fetch_assoc($sth)) $res[]=$row;
    }
    return $res;
}

function db_row($sql){
    $sth=db_query($sql);
    return mysql_fetch_assoc($sth);
}

function db_value($sql){
    $sth=db_query($sql);
    $row=mysql_fetch_row($sth);
    return $row[0];
}

function get_identity($dbh1=NULL){
    $dbh1=db_checkconnect($dbh1);
    return mysql_insert_id($dbh1);
}

function get_db_select($sel=''){
    global $DB;
    if (is_array($_SESSION['sql_sd']) && $_REQUEST['db']!='*'){//check cache
        $arr=$_SESSION['sql_sd'];
    }else{
        $arr=db_array("show databases",NULL,1);
        if (!is_array($arr)){
            $arr=array( 0 => array('Database' => $DB['db']) );
        }
        $_SESSION['sql_sd']=$arr;
    }
    return @sel($arr,'Database',$sel);
}

function get_db_tables($sel='') {
    $query = "show tables from " . $_REQUEST['db'];
    return ($arr=db_array($query, NULL, 1));
}

function chset_select($sel=''){
    global $DBDEF;
    $result='';
    if ($_SESSION['sql_chset']){
        $arr=$_SESSION['sql_chset'];
    }else{
        $arr=db_array("show character set",NULL,1);
        if (!is_array($arr)) $arr=array(array('Charset'=>$DBDEF['chset']));
        $_SESSION['sql_chset']=$arr;
    }

    return @sel($arr,'Charset',$sel);
}

function sel($arr,$n,$sel=''){
    foreach($arr as $a){
        #   echo $a[0];
        $b=$a[$n];
        $res.="<option value='$b' ".($sel && $sel==$b?'selected':'').">$b</option>";
    }
    return $res;
}

function microtime_float(){
    list($usec,$sec)=explode(" ",microtime());
    return ((float)$usec+(float)$sec);
}

/* page nav
 $pg=int($_[0]);     #current page
 $all=int($_[1]);     #total number of items
 $PP=$_[2];      #number if items Per Page
 $ptpl=$_[3];      #page url /ukr/dollar/notes.php?page=    for notes.php
 $show_all=$_[5];           #print Totals?
*/
function get_nav($pg, $all, $PP, $ptpl, $show_all=''){
        $n='&nbsp;';
        $sep=" $n|$n\n";
        if (!$PP) $PP=10;
        $allp=floor($all/$PP+0.999999);

        $pname='';
        $res='';
        $w=array('Less','More','Back','Next','First','Total');

        $sp=$pg-2;
        if($sp<0) $sp=0;
        if($allp-$sp<5 && $allp>=5) $sp=$allp-5;

        $res="";

        if($sp>0){
            $pname=pen($sp-1,$ptpl);
            $res.="<a href='$pname'>$w[0]</a>";
            $res.=$sep;
        }
        for($p_p=$sp;$p_p<$allp && $p_p<$sp+5;$p_p++){
            $first_s=$p_p*$PP+1;
            $last_s=($p_p+1)*$PP;
            $pname=pen($p_p,$ptpl);
            if($last_s>$all){
                $last_s=$all;
            }
            if($p_p==$pg){
                $res.="<b>$first_s..$last_s</b>";
            }else{
                $res.="<a href='$pname'>$first_s..$last_s</a>";
            }
            if($p_p+1<$allp) $res.=$sep;
        }
        if($sp+5<$allp){
            $pname=pen($sp+5,$ptpl);
            $res.="<a href='$pname'>$w[1]</a>";
        }
        $res.=" <br>\n";

        if($pg>0){
            $pname=pen($pg-1,$ptpl);
            $res.="<a href='$pname'>$w[2]</a> $n|$n ";
            $pname=pen(0,$ptpl);
            $res.="<a href='$pname'>$w[4]</a>";
        }
        if($pg>0 && $pg+1<$allp) $res.=$sep;
        if($pg+1<$allp){
            $pname=pen($pg+1,$ptpl);
            $res.="<a href='$pname'>$w[3]</a>";
        }
        if ($show_all) $res.=" <b>($w[5] - $all)</b> ";

        return $res;
    }

function pen($p,$np=''){
    return str_replace('%p%',$p, $np);
}

function killmq($value){
    return is_array($value)?array_map('killmq',$value):stripslashes($value);
}

function savecfg(){
    $v=$_REQUEST['v'];
    $_SESSION['DB']=$v;
    $c=$_REQUEST['c'];
    $_SESSION['CRYPTDB']=$c;
    unset($_SESSION['sql_sd']);

    if ($_REQUEST['rmb']){
        $tm=time()+60*60*24*30;
        setcookie("conn[db]",  $v['db'],$tm);
        setcookie("conn[user]",$v['user'],$tm);
        setcookie("conn[pwd]", $v['pwd'],$tm);
        setcookie("conn[host]",$v['host'],$tm);
        setcookie("conn[port]",$v['port'],$tm);
        setcookie("conn[chset]",$v['chset'],$tm);

        setcookie("cconn[db]",  $c['db'],$tm);
        setcookie("cconn[user]",$c['user'],$tm);
        setcookie("cconn[pwd]", $c['pwd'],$tm);
        setcookie("cconn[host]",$c['host'],$tm);
        setcookie("cconn[port]",$c['port'],$tm);
        setcookie("cconn[chset]",$c['chset'],$tm);
    }else{
        setcookie("conn[db]",  FALSE,-1);
        setcookie("conn[user]",FALSE,-1);
        setcookie("conn[pwd]", FALSE,-1);
        setcookie("conn[host]",FALSE,-1);
        setcookie("conn[port]",FALSE,-1);
        setcookie("conn[chset]",FALSE,-1);


        setcookie("cconn[db]",  FALSE,-1);
        setcookie("cconn[user]",FALSE,-1);
        setcookie("cconn[pwd]", FALSE,-1);
        setcookie("cconn[host]",FALSE,-1);
        setcookie("cconn[port]",FALSE,-1);
        setcookie("cconn[chset]",FALSE,-1);
    }
}

//during login only - from cookies or use defaults;
function loadcfg(){
    global $DBDEF, $CRYPTDBDEF;

    if( isset($_COOKIE['conn']) ){
        $a=$_COOKIE['conn'];
        $_SESSION['DB']=$_COOKIE['conn'];
    }else{
        $_SESSION['DB']=$DBDEF;
    }
    if (!strlen($_SESSION['DB']['chset'])) $_SESSION['DB']['chset']=$DBDEF['chset'];#don't allow empty charset


    if( isset($_COOKIE['cconn']) ){
        $a=$_COOKIE['cconn'];
        $_SESSION['CRYPTDB']=$_COOKIE['cconn'];
    }else{
        $_SESSION['CRYPTDB']=$CRYPTDBDEF;
    }
    if (!strlen($_SESSION['CRYPTDB']['chset'])) $_SESSION['CRYPTDB']['chset']=$CRYPTDBDEF['chset'];#don't allow empty charset

}

//each time - from session to $DB_*
function loadsess(){
    global $DB, $CRYPTDB;

    $DB=$_SESSION['DB'];

    $rdb=$_REQUEST['db'];
    if ($rdb=='*') $rdb='';
    if ($rdb) {
        $DB['db']=$rdb;
    }

    $CRYPTDB=$_SESSION['CRYPTDB'];

    $cryptrdb=$_REQUEST['db'];
    if ($cryptrdb=='*') $cryptrdb='';
    if ($cryptrdb) {
        $CRYPTDB['db']=$cryptrdb;
    }


}

function print_export(){
    global $self,$xurl,$DB;
    $t=$_REQUEST['t'];
    $l=($t)?"Table $t":"whole DB";
    print_header();
?>
<center>
<h3>Export <?php echo $l?></h3>
<div class="frm">
<input type="checkbox" name="s" value="1" checked> Structure<br>
<input type="checkbox" name="d" value="1" checked> Data<br><br>
<div><label><input type="radio" name="et" value="" checked> .sql</label>&nbsp;</div>
<div>
<?php if ($t && !strpos($t,',')){?>
 <label><input type="radio" name="et" value="csv"> .csv (Excel style, data only and for one table only)</label>
<?php }else{?>
<label>&nbsp;( ) .csv</label> <small>(to export as csv - go to 'show tables' and export just ONE table)</small>
<?php }?>
</div>
<br>
<div><label><input type="checkbox" name="gz" value="1"> compress as .gz</label></div>
<br>
<input type="hidden" name="doex" value="1">
<input type="hidden" name="t" value="<?php echo $t?>">
<input type="submit" value=" Download "><input type="button" value=" Cancel " onclick="window.location='<?php echo $self.'?'.$xurl.'&db='.$DB['db']?>'">
</div>
</center>
<?php
    print_footer();
    exit;
}


function do_export_table($t='',$isvar=0,$MAXI=838860){
    global $D;
    set_time_limit(600);

    if($_REQUEST['s']){
        $sth=db_query("show create table `$t`");
        $row=mysql_fetch_row($sth);
        $ct=preg_replace("/\n\r|\r\n|\n|\r/",$D,$row[1]);
        ex_w("DROP TABLE IF EXISTS `$t`;$D$ct;$D$D");
    }

    if ($_REQUEST['d']){
        $exsql='';
        ex_w("/*!40000 ALTER TABLE `$t` DISABLE KEYS */;$D");
        $sth=db_query("select * from `$t`");
        while($row=mysql_fetch_row($sth)){
            $values='';
            foreach($row as $v) $values.=(($values)?',':'').dbq($v);
            $exsql.=(($exsql)?',':'')."(".$values.")";
            if (strlen($exsql)>$MAXI) {
                ex_w("INSERT INTO `$t` VALUES $exsql;$D");$exsql='';
            }
        }
        if ($exsql) ex_w("INSERT INTO `$t` VALUES $exsql;$D");
        ex_w("/*!40000 ALTER TABLE `$t` ENABLE KEYS */;$D$D");
    }
    flush();
}

function ex_hdr($ct,$fn){
    header("Content-type: $ct");
    header("Content-Disposition: attachment; filename=\"$fn\"");
}
function ex_start(){
    global $ex_isgz,$ex_gz,$ex_tmpf;
    if ($ex_isgz){
        $ex_tmpf=tempnam(sys_get_temp_dir(),'pma').'.gz';
        if (!($ex_gz=gzopen($ex_tmpf,'wb9'))) die("Error trying to create gz tmp file");
    }
}
function ex_w($s){
    global $ex_isgz,$ex_gz;
    if ($ex_isgz){
        gzwrite($ex_gz,$s,strlen($s));
    }else{
        echo $s;
    }
}
function ex_end(){
    global $ex_isgz,$ex_gz,$ex_tmpf;
    if ($ex_isgz){
        gzclose($ex_gz);
        readfile($ex_tmpf);
    }
}

// multiple SQL statements splitter
function do_multi_sql($insql,$fname=''){
    set_time_limit(600);

    $sql='';
    $ochar='';
    $is_cmt='';
    $GLOBALS['insql_done']=0;
    while ($str=get_next_chunk($insql,$fname)){
        $opos=-strlen($ochar);
        $cur_pos=0;
        $i=strlen($str);
        while ($i--){
            if ($ochar){
                list($clchar, $clpos)=get_close_char($str, $opos+strlen($ochar), $ochar);
                if ( $clchar ) {
                    if ($ochar=='--' || $ochar=='#' || $is_cmt ){
                        $sql.=substr($str, $cur_pos, $opos-$cur_pos );
                    }else{
                        $sql.=substr($str, $cur_pos, $clpos+strlen($clchar)-$cur_pos );
                    }
                    $cur_pos=$clpos+strlen($clchar);
                    $ochar='';
                    $opos=0;
                }else{
                    $sql.=substr($str, $cur_pos);
                    break;
                }
            }else{
                list($ochar, $opos)=get_open_char($str, $cur_pos);
                if ($ochar==';'){
                    $sql.=substr($str, $cur_pos, $opos-$cur_pos+1);
                    if (!do_one_sql($sql)) return 0;
                    $sql='';
                    $cur_pos=$opos+strlen($ochar);
                    $ochar='';
                    $opos=0;
                }elseif(!$ochar) {
                    $sql.=substr($str, $cur_pos);
                    break;
                }else{
                    $is_cmt=0;if ($ochar=='/*' && substr($str, $opos, 3)!='/*!') $is_cmt=1;
                }
            }
        }
    }

    if ($sql){
        if (!do_one_sql($sql)) return 0;
        $sql='';
    }
    return 1;
}

//read from insql var or file
function get_next_chunk($insql, $fname){
    global $LFILE, $insql_done;
    if ($insql) {
        if ($insql_done){
            return '';
        }else{
            $insql_done=1;
            return $insql;
        }
    }
    if (!$fname) return '';
    if (!$LFILE){
        $LFILE=fopen($fname,"r+b") or die("Can't open [$fname] file $!");
    }
    return fread($LFILE, 64*1024);
}

function get_open_char($str, $pos){
    if ( preg_match("/(\/\*|^--|(?<=\s)--|#|'|\"|;)/", $str, $m, PREG_OFFSET_CAPTURE, $pos) ) {
        $ochar=$m[1][0];
        $opos=$m[1][1];
    }
    return array($ochar, $opos);
}

#RECURSIVE!
function get_close_char($str, $pos, $ochar){
    $aCLOSE=array(
        '\'' => '(?<!\\\\)\'|(\\\\+)\'',
        '"' => '(?<!\\\\)"',
        '/*' => '\*\/',
        '#' => '[\r\n]+',
        '--' => '[\r\n]+',
    );
    if ( $aCLOSE[$ochar] && preg_match("/(".$aCLOSE[$ochar].")/", $str, $m, PREG_OFFSET_CAPTURE, $pos ) ) {
        $clchar=$m[1][0];
        $clpos=$m[1][1];
        $sl=strlen($m[2][0]);
        if ($ochar=="'" && $sl){
            if ($sl % 2){ #don't count as CLOSE char if number of slashes before ' ODD
                list($clchar, $clpos)=get_close_char($str, $clpos+strlen($clchar), $ochar);
            }else{
                $clpos+=strlen($clchar)-1;$clchar="'";#correction
            }
        }
    }
    return array($clchar, $clpos);
}

function do_one_sql($sql){
    global $last_sth,$last_sql,$MAX_ROWS_PER_PAGE,$page,$is_limited_sql;
    $sql=trim($sql);
    $sql=preg_replace("/;$/","",$sql);
    if ($sql){
        $last_sql=$sql;$is_limited_sql=0;
        if (preg_match("/^select/i",$sql) && !preg_match("/limit +\d+/i", $sql)){
            $offset=$page*$MAX_ROWS_PER_PAGE;
            $sql.=" LIMIT $offset,$MAX_ROWS_PER_PAGE";
            $is_limited_sql=1;
        }
        $last_sth=db_query($sql,0,'noerr');
        return $last_sth;
    }
    return 1;
}


function to_csv_row($adata){
    global $D;
    $r='';
    foreach ($adata as $a){
        $r.=(($r)?",":"").qstr($a);
    }
    return $r.$D;
}
function qstr($s){
    $s=nl2br($s);
    $s=str_replace('"','""',$s);
    return '"'.$s.'"';
}

function get_rand_str($len){
    $result='';
    $chars=preg_split('//','ABCDEFabcdef0123456789');
    for($i=0;$i<$len;$i++) $result.=$chars[rand(0,count($chars)-1)];
    return $result;
}

function check_xss(){
    global $self;
    if ($_SESSION['XSS']!=trim($_REQUEST['XSS'])){
        unset($_SESSION['XSS']);
        header("location: $self");
        exit;
    }
}

function rw($s){#for debug
 echo $s."<br>\n";
}

?>
