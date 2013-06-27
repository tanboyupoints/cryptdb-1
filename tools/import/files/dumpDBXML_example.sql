<?xml version="1.0"?>
<mysqldump xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<database name="carlos">
</database>
<database name="cryptdb_udf">
</database>
<database name="cryptdbtest">
	<table_structure name="t1">
		<field Field="id" Type="int(11)" Null="YES" Key="" Extra="" Comment="" />
		<field Field="name" Type="text" Null="YES" Key="" Extra="" Comment="" />
		<field Field="salary" Type="int(11)" Null="YES" Key="" Extra="" Comment="" />
		<options Name="t1" Engine="InnoDB" Version="10" Row_format="Compact" Rows="4" Avg_row_length="4096" Data_length="16384" Max_data_length="0" Index_length="0" Data_free="10485760" Create_time="2013-06-24 16:32:34" Collation="latin1_swedish_ci" Create_options="" Comment="" />
	</table_structure>
	<table_data name="t1">
	<row>
		<field name="id">1</field>
		<field name="name">alice</field>
		<field name="salary">100</field>
	</row>
	<row>
		<field name="id">2</field>
		<field name="name">bob</field>
		<field name="salary">200</field>
	</row>
	<row>
		<field name="id">0</field>
		<field name="name">chris</field>
		<field name="salary">0</field>
	</row>
	<row>
		<field name="id">4</field>
		<field name="name">dan dennis</field>
		<field name="salary">0</field>
	</row>
	</table_data>
</database>
<database name="cryptdbtest_control">
</database>
<database name="mysql">
	<table_structure name="general_log">
		<field Field="event_time" Type="timestamp" Null="NO" Key="" Default="CURRENT_TIMESTAMP" Extra="on update CURRENT_TIMESTAMP" Comment="" />
		<field Field="user_host" Type="mediumtext" Null="NO" Key="" Extra="" Comment="" />
		<field Field="thread_id" Type="int(11)" Null="NO" Key="" Extra="" Comment="" />
		<field Field="server_id" Type="int(10) unsigned" Null="NO" Key="" Extra="" Comment="" />
		<field Field="command_type" Type="varchar(64)" Null="NO" Key="" Extra="" Comment="" />
		<field Field="argument" Type="mediumtext" Null="NO" Key="" Extra="" Comment="" />
		<options Name="general_log" Engine="CSV" Version="10" Row_format="Dynamic" Rows="2" Avg_row_length="0" Data_length="0" Max_data_length="0" Index_length="0" Data_free="0" Collation="utf8_general_ci" Create_options="" Comment="General log" />
	</table_structure>
	<table_structure name="slow_log">
		<field Field="start_time" Type="timestamp" Null="NO" Key="" Default="CURRENT_TIMESTAMP" Extra="on update CURRENT_TIMESTAMP" Comment="" />
		<field Field="user_host" Type="mediumtext" Null="NO" Key="" Extra="" Comment="" />
		<field Field="query_time" Type="time" Null="NO" Key="" Extra="" Comment="" />
		<field Field="lock_time" Type="time" Null="NO" Key="" Extra="" Comment="" />
		<field Field="rows_sent" Type="int(11)" Null="NO" Key="" Extra="" Comment="" />
		<field Field="rows_examined" Type="int(11)" Null="NO" Key="" Extra="" Comment="" />
		<field Field="db" Type="varchar(512)" Null="NO" Key="" Extra="" Comment="" />
		<field Field="last_insert_id" Type="int(11)" Null="NO" Key="" Extra="" Comment="" />
		<field Field="insert_id" Type="int(11)" Null="NO" Key="" Extra="" Comment="" />
		<field Field="server_id" Type="int(10) unsigned" Null="NO" Key="" Extra="" Comment="" />
		<field Field="sql_text" Type="mediumtext" Null="NO" Key="" Extra="" Comment="" />
		<options Name="slow_log" Engine="CSV" Version="10" Row_format="Dynamic" Rows="2" Avg_row_length="0" Data_length="0" Max_data_length="0" Index_length="0" Data_free="0" Collation="utf8_general_ci" Create_options="" Comment="Slow log" />
	</table_structure>
	<table_structure name="columns_priv">
		<field Field="Host" Type="char(60)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Db" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="User" Type="char(16)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Table_name" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Column_name" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Timestamp" Type="timestamp" Null="NO" Key="" Default="CURRENT_TIMESTAMP" Extra="on update CURRENT_TIMESTAMP" Comment="" />
		<field Field="Column_priv" Type="set('Select','Insert','Update','References')" Null="NO" Key="" Default="" Extra="" Comment="" />
		<key Table="columns_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Host" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="columns_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="2" Column_name="Db" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="columns_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="3" Column_name="User" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="columns_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="4" Column_name="Table_name" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="columns_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="5" Column_name="Column_name" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="columns_priv" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="227994731135631359" Index_length="4096" Data_free="0" Create_time="2013-06-14 00:07:12" Update_time="2013-06-14 00:07:12" Collation="utf8_bin" Create_options="" Comment="Column privileges" />
	</table_structure>
	<table_data name="columns_priv">
	</table_data>
	<table_structure name="db">
		<field Field="Host" Type="char(60)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Db" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="User" Type="char(16)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Select_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Insert_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Update_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Delete_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Drop_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Grant_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="References_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Index_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Alter_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_tmp_table_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Lock_tables_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_view_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Show_view_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_routine_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Alter_routine_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Execute_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Event_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Trigger_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<key Table="db" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Host" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="db" Non_unique="0" Key_name="PRIMARY" Seq_in_index="2" Column_name="Db" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="db" Non_unique="0" Key_name="PRIMARY" Seq_in_index="3" Column_name="User" Collation="A" Cardinality="2" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="db" Non_unique="1" Key_name="User" Seq_in_index="1" Column_name="User" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="db" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="2" Avg_row_length="440" Data_length="880" Max_data_length="123848989752688639" Index_length="5120" Data_free="0" Create_time="2013-06-14 00:07:20" Update_time="2013-06-14 00:07:20" Collation="utf8_bin" Create_options="" Comment="Database privileges" />
	</table_structure>
	<table_data name="db">
	<row>
		<field name="Host">%</field>
		<field name="Db">test</field>
		<field name="User"></field>
		<field name="Select_priv">Y</field>
		<field name="Insert_priv">Y</field>
		<field name="Update_priv">Y</field>
		<field name="Delete_priv">Y</field>
		<field name="Create_priv">Y</field>
		<field name="Drop_priv">Y</field>
		<field name="Grant_priv">N</field>
		<field name="References_priv">Y</field>
		<field name="Index_priv">Y</field>
		<field name="Alter_priv">Y</field>
		<field name="Create_tmp_table_priv">Y</field>
		<field name="Lock_tables_priv">Y</field>
		<field name="Create_view_priv">Y</field>
		<field name="Show_view_priv">Y</field>
		<field name="Create_routine_priv">Y</field>
		<field name="Alter_routine_priv">N</field>
		<field name="Execute_priv">N</field>
		<field name="Event_priv">Y</field>
		<field name="Trigger_priv">Y</field>
	</row>
	<row>
		<field name="Host">%</field>
		<field name="Db">test\_%</field>
		<field name="User"></field>
		<field name="Select_priv">Y</field>
		<field name="Insert_priv">Y</field>
		<field name="Update_priv">Y</field>
		<field name="Delete_priv">Y</field>
		<field name="Create_priv">Y</field>
		<field name="Drop_priv">Y</field>
		<field name="Grant_priv">N</field>
		<field name="References_priv">Y</field>
		<field name="Index_priv">Y</field>
		<field name="Alter_priv">Y</field>
		<field name="Create_tmp_table_priv">Y</field>
		<field name="Lock_tables_priv">Y</field>
		<field name="Create_view_priv">Y</field>
		<field name="Show_view_priv">Y</field>
		<field name="Create_routine_priv">Y</field>
		<field name="Alter_routine_priv">N</field>
		<field name="Execute_priv">N</field>
		<field name="Event_priv">Y</field>
		<field name="Trigger_priv">Y</field>
	</row>
	</table_data>
	<table_structure name="event">
		<field Field="db" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="name" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="body" Type="longblob" Null="NO" Key="" Extra="" Comment="" />
		<field Field="definer" Type="char(77)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="execute_at" Type="datetime" Null="YES" Key="" Extra="" Comment="" />
		<field Field="interval_value" Type="int(11)" Null="YES" Key="" Extra="" Comment="" />
		<field Field="interval_field" Type="enum('YEAR','QUARTER','MONTH','DAY','HOUR','MINUTE','WEEK','SECOND','MICROSECOND','YEAR_MONTH','DAY_HOUR','DAY_MINUTE','DAY_SECOND','HOUR_MINUTE','HOUR_SECOND','MINUTE_SECOND','DAY_MICROSECOND','HOUR_MICROSECOND','MINUTE_MICROSECOND','SECOND_MICROSECOND')" Null="YES" Key="" Extra="" Comment="" />
		<field Field="created" Type="timestamp" Null="NO" Key="" Default="CURRENT_TIMESTAMP" Extra="on update CURRENT_TIMESTAMP" Comment="" />
		<field Field="modified" Type="timestamp" Null="NO" Key="" Default="0000-00-00 00:00:00" Extra="" Comment="" />
		<field Field="last_executed" Type="datetime" Null="YES" Key="" Extra="" Comment="" />
		<field Field="starts" Type="datetime" Null="YES" Key="" Extra="" Comment="" />
		<field Field="ends" Type="datetime" Null="YES" Key="" Extra="" Comment="" />
		<field Field="status" Type="enum('ENABLED','DISABLED','SLAVESIDE_DISABLED')" Null="NO" Key="" Default="ENABLED" Extra="" Comment="" />
		<field Field="on_completion" Type="enum('DROP','PRESERVE')" Null="NO" Key="" Default="DROP" Extra="" Comment="" />
		<field Field="sql_mode" Type="set('REAL_AS_FLOAT','PIPES_AS_CONCAT','ANSI_QUOTES','IGNORE_SPACE','NOT_USED','ONLY_FULL_GROUP_BY','NO_UNSIGNED_SUBTRACTION','NO_DIR_IN_CREATE','POSTGRESQL','ORACLE','MSSQL','DB2','MAXDB','NO_KEY_OPTIONS','NO_TABLE_OPTIONS','NO_FIELD_OPTIONS','MYSQL323','MYSQL40','ANSI','NO_AUTO_VALUE_ON_ZERO','NO_BACKSLASH_ESCAPES','STRICT_TRANS_TABLES','STRICT_ALL_TABLES','NO_ZERO_IN_DATE','NO_ZERO_DATE','INVALID_DATES','ERROR_FOR_DIVISION_BY_ZERO','TRADITIONAL','NO_AUTO_CREATE_USER','HIGH_NOT_PRECEDENCE','NO_ENGINE_SUBSTITUTION','PAD_CHAR_TO_FULL_LENGTH')" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="comment" Type="char(64)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="originator" Type="int(10) unsigned" Null="NO" Key="" Extra="" Comment="" />
		<field Field="time_zone" Type="char(64)" Null="NO" Key="" Default="SYSTEM" Extra="" Comment="" />
		<field Field="character_set_client" Type="char(32)" Null="YES" Key="" Extra="" Comment="" />
		<field Field="collation_connection" Type="char(32)" Null="YES" Key="" Extra="" Comment="" />
		<field Field="db_collation" Type="char(32)" Null="YES" Key="" Extra="" Comment="" />
		<field Field="body_utf8" Type="longblob" Null="YES" Key="" Extra="" Comment="" />
		<key Table="event" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="db" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="event" Non_unique="0" Key_name="PRIMARY" Seq_in_index="2" Column_name="name" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="event" Engine="MyISAM" Version="10" Row_format="Dynamic" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="281474976710655" Index_length="2048" Data_free="0" Create_time="2013-06-14 00:07:19" Update_time="2013-06-14 00:07:19" Collation="utf8_general_ci" Create_options="" Comment="Events" />
	</table_structure>
	<table_structure name="func">
		<field Field="name" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="ret" Type="tinyint(1)" Null="NO" Key="" Default="0" Extra="" Comment="" />
		<field Field="dl" Type="char(128)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="type" Type="enum('function','aggregate')" Null="NO" Key="" Extra="" Comment="" />
		<key Table="func" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="name" Collation="A" Cardinality="7" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="func" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="7" Avg_row_length="579" Data_length="4053" Max_data_length="162974011515469823" Index_length="2048" Data_free="0" Create_time="2013-06-14 00:07:14" Update_time="2013-06-24 16:31:31" Check_time="2013-06-17 22:24:13" Collation="utf8_bin" Create_options="" Comment="User defined functions" />
	</table_structure>
	<table_data name="func">
	<row>
		<field name="name">searchSWP</field>
		<field name="ret">2</field>
		<field name="dl">edb.so</field>
		<field name="type">function</field>
	</row>
	<row>
		<field name="name">agg</field>
		<field name="ret">0</field>
		<field name="dl">edb.so</field>
		<field name="type">aggregate</field>
	</row>
	<row>
		<field name="name">func_add_set</field>
		<field name="ret">0</field>
		<field name="dl">edb.so</field>
		<field name="type">function</field>
	</row>
	<row>
		<field name="name">decrypt_text_det</field>
		<field name="ret">0</field>
		<field name="dl">edb.so</field>
		<field name="type">function</field>
	</row>
	<row>
		<field name="name">decrypt_int_det</field>
		<field name="ret">2</field>
		<field name="dl">edb.so</field>
		<field name="type">function</field>
	</row>
	<row>
		<field name="name">decrypt_text_sem</field>
		<field name="ret">0</field>
		<field name="dl">edb.so</field>
		<field name="type">function</field>
	</row>
	<row>
		<field name="name">decrypt_int_sem</field>
		<field name="ret">2</field>
		<field name="dl">edb.so</field>
		<field name="type">function</field>
	</row>
	</table_data>
	<table_structure name="help_category">
		<field Field="help_category_id" Type="smallint(5) unsigned" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="name" Type="char(64)" Null="NO" Key="UNI" Extra="" Comment="" />
		<field Field="parent_category_id" Type="smallint(5) unsigned" Null="YES" Key="" Extra="" Comment="" />
		<field Field="url" Type="char(128)" Null="NO" Key="" Extra="" Comment="" />
		<key Table="help_category" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="help_category_id" Collation="A" Cardinality="40" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="help_category" Non_unique="0" Key_name="name" Seq_in_index="1" Column_name="name" Collation="A" Cardinality="40" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="help_category" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="40" Avg_row_length="581" Data_length="23240" Max_data_length="163536961468891135" Index_length="3072" Data_free="0" Create_time="2013-06-14 00:06:54" Update_time="2013-06-14 00:06:56" Collation="utf8_general_ci" Create_options="" Comment="help categories" />
	</table_structure>
	<table_data name="help_category">
	<row>
		<field name="help_category_id">1</field>
		<field name="name">Geographic</field>
		<field name="parent_category_id">0</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">2</field>
		<field name="name">Polygon properties</field>
		<field name="parent_category_id">35</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">3</field>
		<field name="name">WKT</field>
		<field name="parent_category_id">35</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">4</field>
		<field name="name">Numeric Functions</field>
		<field name="parent_category_id">39</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">5</field>
		<field name="name">Plugins</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">6</field>
		<field name="name">MBR</field>
		<field name="parent_category_id">35</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">7</field>
		<field name="name">Control flow functions</field>
		<field name="parent_category_id">39</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">8</field>
		<field name="name">Transactions</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">9</field>
		<field name="name">Help Metadata</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">10</field>
		<field name="name">Account Management</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">11</field>
		<field name="name">Point properties</field>
		<field name="parent_category_id">35</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">12</field>
		<field name="name">Encryption Functions</field>
		<field name="parent_category_id">39</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">13</field>
		<field name="name">LineString properties</field>
		<field name="parent_category_id">35</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">14</field>
		<field name="name">Miscellaneous Functions</field>
		<field name="parent_category_id">39</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">15</field>
		<field name="name">Logical operators</field>
		<field name="parent_category_id">39</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">16</field>
		<field name="name">Functions and Modifiers for Use with GROUP BY</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">17</field>
		<field name="name">Information Functions</field>
		<field name="parent_category_id">39</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">18</field>
		<field name="name">Storage Engines</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">19</field>
		<field name="name">Comparison operators</field>
		<field name="parent_category_id">39</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">20</field>
		<field name="name">Bit Functions</field>
		<field name="parent_category_id">39</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">21</field>
		<field name="name">Table Maintenance</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">22</field>
		<field name="name">User-Defined Functions</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">23</field>
		<field name="name">Data Types</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">24</field>
		<field name="name">Compound Statements</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">25</field>
		<field name="name">Geometry constructors</field>
		<field name="parent_category_id">35</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">26</field>
		<field name="name">GeometryCollection properties</field>
		<field name="parent_category_id">1</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">27</field>
		<field name="name">Administration</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">28</field>
		<field name="name">Data Manipulation</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">29</field>
		<field name="name">Utility</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">30</field>
		<field name="name">Language Structure</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">31</field>
		<field name="name">Geometry relations</field>
		<field name="parent_category_id">35</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">32</field>
		<field name="name">Date and Time Functions</field>
		<field name="parent_category_id">39</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">33</field>
		<field name="name">WKB</field>
		<field name="parent_category_id">35</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">34</field>
		<field name="name">Procedures</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">35</field>
		<field name="name">Geographic Features</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">36</field>
		<field name="name">Contents</field>
		<field name="parent_category_id">0</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">37</field>
		<field name="name">Geometry properties</field>
		<field name="parent_category_id">35</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">38</field>
		<field name="name">String Functions</field>
		<field name="parent_category_id">39</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">39</field>
		<field name="name">Functions</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_category_id">40</field>
		<field name="name">Data Definition</field>
		<field name="parent_category_id">36</field>
		<field name="url"></field>
	</row>
	</table_data>
	<table_structure name="help_keyword">
		<field Field="help_keyword_id" Type="int(10) unsigned" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="name" Type="char(64)" Null="NO" Key="UNI" Extra="" Comment="" />
		<key Table="help_keyword" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="help_keyword_id" Collation="A" Cardinality="467" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="help_keyword" Non_unique="0" Key_name="name" Seq_in_index="1" Column_name="name" Collation="A" Cardinality="467" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="help_keyword" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="467" Avg_row_length="197" Data_length="91999" Max_data_length="55450570411999231" Index_length="17408" Data_free="0" Create_time="2013-06-14 00:06:54" Update_time="2013-06-14 00:06:56" Collation="utf8_general_ci" Create_options="" Comment="help keywords" />
	</table_structure>
	<table_data name="help_keyword">
	<row>
		<field name="help_keyword_id">0</field>
		<field name="name">JOIN</field>
	</row>
	<row>
		<field name="help_keyword_id">1</field>
		<field name="name">HOST</field>
	</row>
	<row>
		<field name="help_keyword_id">2</field>
		<field name="name">REPEAT</field>
	</row>
	<row>
		<field name="help_keyword_id">3</field>
		<field name="name">SERIALIZABLE</field>
	</row>
	<row>
		<field name="help_keyword_id">4</field>
		<field name="name">REPLACE</field>
	</row>
	<row>
		<field name="help_keyword_id">5</field>
		<field name="name">AT</field>
	</row>
	<row>
		<field name="help_keyword_id">6</field>
		<field name="name">SCHEDULE</field>
	</row>
	<row>
		<field name="help_keyword_id">7</field>
		<field name="name">RETURNS</field>
	</row>
	<row>
		<field name="help_keyword_id">8</field>
		<field name="name">STARTS</field>
	</row>
	<row>
		<field name="help_keyword_id">9</field>
		<field name="name">MASTER_SSL_CA</field>
	</row>
	<row>
		<field name="help_keyword_id">10</field>
		<field name="name">NCHAR</field>
	</row>
	<row>
		<field name="help_keyword_id">11</field>
		<field name="name">ONLY</field>
	</row>
	<row>
		<field name="help_keyword_id">12</field>
		<field name="name">COLUMNS</field>
	</row>
	<row>
		<field name="help_keyword_id">13</field>
		<field name="name">COMPLETION</field>
	</row>
	<row>
		<field name="help_keyword_id">14</field>
		<field name="name">WORK</field>
	</row>
	<row>
		<field name="help_keyword_id">15</field>
		<field name="name">DATETIME</field>
	</row>
	<row>
		<field name="help_keyword_id">16</field>
		<field name="name">MODE</field>
	</row>
	<row>
		<field name="help_keyword_id">17</field>
		<field name="name">OPEN</field>
	</row>
	<row>
		<field name="help_keyword_id">18</field>
		<field name="name">INTEGER</field>
	</row>
	<row>
		<field name="help_keyword_id">19</field>
		<field name="name">ESCAPE</field>
	</row>
	<row>
		<field name="help_keyword_id">20</field>
		<field name="name">VALUE</field>
	</row>
	<row>
		<field name="help_keyword_id">21</field>
		<field name="name">MASTER_SSL_VERIFY_SERVER_CERT</field>
	</row>
	<row>
		<field name="help_keyword_id">22</field>
		<field name="name">SQL_BIG_RESULT</field>
	</row>
	<row>
		<field name="help_keyword_id">23</field>
		<field name="name">DROP</field>
	</row>
	<row>
		<field name="help_keyword_id">24</field>
		<field name="name">GEOMETRYCOLLECTIONFROMWKB</field>
	</row>
	<row>
		<field name="help_keyword_id">25</field>
		<field name="name">EVENTS</field>
	</row>
	<row>
		<field name="help_keyword_id">26</field>
		<field name="name">MONTH</field>
	</row>
	<row>
		<field name="help_keyword_id">27</field>
		<field name="name">PROFILES</field>
	</row>
	<row>
		<field name="help_keyword_id">28</field>
		<field name="name">DUPLICATE</field>
	</row>
	<row>
		<field name="help_keyword_id">29</field>
		<field name="name">REPLICATION</field>
	</row>
	<row>
		<field name="help_keyword_id">30</field>
		<field name="name">UNLOCK</field>
	</row>
	<row>
		<field name="help_keyword_id">31</field>
		<field name="name">INNODB</field>
	</row>
	<row>
		<field name="help_keyword_id">32</field>
		<field name="name">YEAR_MONTH</field>
	</row>
	<row>
		<field name="help_keyword_id">33</field>
		<field name="name">SUBJECT</field>
	</row>
	<row>
		<field name="help_keyword_id">34</field>
		<field name="name">PREPARE</field>
	</row>
	<row>
		<field name="help_keyword_id">35</field>
		<field name="name">LOCK</field>
	</row>
	<row>
		<field name="help_keyword_id">36</field>
		<field name="name">NDB</field>
	</row>
	<row>
		<field name="help_keyword_id">37</field>
		<field name="name">CHECK</field>
	</row>
	<row>
		<field name="help_keyword_id">38</field>
		<field name="name">FULL</field>
	</row>
	<row>
		<field name="help_keyword_id">39</field>
		<field name="name">INT4</field>
	</row>
	<row>
		<field name="help_keyword_id">40</field>
		<field name="name">BY</field>
	</row>
	<row>
		<field name="help_keyword_id">41</field>
		<field name="name">NO</field>
	</row>
	<row>
		<field name="help_keyword_id">42</field>
		<field name="name">MINUTE</field>
	</row>
	<row>
		<field name="help_keyword_id">43</field>
		<field name="name">PARTITION</field>
	</row>
	<row>
		<field name="help_keyword_id">44</field>
		<field name="name">DATA</field>
	</row>
	<row>
		<field name="help_keyword_id">45</field>
		<field name="name">DAY</field>
	</row>
	<row>
		<field name="help_keyword_id">46</field>
		<field name="name">SHARE</field>
	</row>
	<row>
		<field name="help_keyword_id">47</field>
		<field name="name">REAL</field>
	</row>
	<row>
		<field name="help_keyword_id">48</field>
		<field name="name">SEPARATOR</field>
	</row>
	<row>
		<field name="help_keyword_id">49</field>
		<field name="name">MESSAGE_TEXT</field>
	</row>
	<row>
		<field name="help_keyword_id">50</field>
		<field name="name">MASTER_HEARTBEAT_PERIOD</field>
	</row>
	<row>
		<field name="help_keyword_id">51</field>
		<field name="name">DELETE</field>
	</row>
	<row>
		<field name="help_keyword_id">52</field>
		<field name="name">ON</field>
	</row>
	<row>
		<field name="help_keyword_id">53</field>
		<field name="name">COLUMN_NAME</field>
	</row>
	<row>
		<field name="help_keyword_id">54</field>
		<field name="name">CONNECTION</field>
	</row>
	<row>
		<field name="help_keyword_id">55</field>
		<field name="name">CLOSE</field>
	</row>
	<row>
		<field name="help_keyword_id">56</field>
		<field name="name">X509</field>
	</row>
	<row>
		<field name="help_keyword_id">57</field>
		<field name="name">USE</field>
	</row>
	<row>
		<field name="help_keyword_id">58</field>
		<field name="name">SUBCLASS_ORIGIN</field>
	</row>
	<row>
		<field name="help_keyword_id">59</field>
		<field name="name">WHERE</field>
	</row>
	<row>
		<field name="help_keyword_id">60</field>
		<field name="name">PRIVILEGES</field>
	</row>
	<row>
		<field name="help_keyword_id">61</field>
		<field name="name">SPATIAL</field>
	</row>
	<row>
		<field name="help_keyword_id">62</field>
		<field name="name">EVENT</field>
	</row>
	<row>
		<field name="help_keyword_id">63</field>
		<field name="name">SUPER</field>
	</row>
	<row>
		<field name="help_keyword_id">64</field>
		<field name="name">SQL_BUFFER_RESULT</field>
	</row>
	<row>
		<field name="help_keyword_id">65</field>
		<field name="name">IGNORE</field>
	</row>
	<row>
		<field name="help_keyword_id">66</field>
		<field name="name">SHA2</field>
	</row>
	<row>
		<field name="help_keyword_id">67</field>
		<field name="name">QUICK</field>
	</row>
	<row>
		<field name="help_keyword_id">68</field>
		<field name="name">SIGNED</field>
	</row>
	<row>
		<field name="help_keyword_id">69</field>
		<field name="name">OFFLINE</field>
	</row>
	<row>
		<field name="help_keyword_id">70</field>
		<field name="name">SECURITY</field>
	</row>
	<row>
		<field name="help_keyword_id">71</field>
		<field name="name">AUTOEXTEND_SIZE</field>
	</row>
	<row>
		<field name="help_keyword_id">72</field>
		<field name="name">NDBCLUSTER</field>
	</row>
	<row>
		<field name="help_keyword_id">73</field>
		<field name="name">POLYGONFROMWKB</field>
	</row>
	<row>
		<field name="help_keyword_id">74</field>
		<field name="name">FALSE</field>
	</row>
	<row>
		<field name="help_keyword_id">75</field>
		<field name="name">LEVEL</field>
	</row>
	<row>
		<field name="help_keyword_id">76</field>
		<field name="name">FORCE</field>
	</row>
	<row>
		<field name="help_keyword_id">77</field>
		<field name="name">BINARY</field>
	</row>
	<row>
		<field name="help_keyword_id">78</field>
		<field name="name">TO</field>
	</row>
	<row>
		<field name="help_keyword_id">79</field>
		<field name="name">CHANGE</field>
	</row>
	<row>
		<field name="help_keyword_id">80</field>
		<field name="name">CURRENT_USER</field>
	</row>
	<row>
		<field name="help_keyword_id">81</field>
		<field name="name">HOUR_MINUTE</field>
	</row>
	<row>
		<field name="help_keyword_id">82</field>
		<field name="name">UPDATE</field>
	</row>
	<row>
		<field name="help_keyword_id">83</field>
		<field name="name">PRESERVE</field>
	</row>
	<row>
		<field name="help_keyword_id">84</field>
		<field name="name">TABLE_NAME</field>
	</row>
	<row>
		<field name="help_keyword_id">85</field>
		<field name="name">INTO</field>
	</row>
	<row>
		<field name="help_keyword_id">86</field>
		<field name="name">FEDERATED</field>
	</row>
	<row>
		<field name="help_keyword_id">87</field>
		<field name="name">VARYING</field>
	</row>
	<row>
		<field name="help_keyword_id">88</field>
		<field name="name">MAX_SIZE</field>
	</row>
	<row>
		<field name="help_keyword_id">89</field>
		<field name="name">HOUR_SECOND</field>
	</row>
	<row>
		<field name="help_keyword_id">90</field>
		<field name="name">VARIABLE</field>
	</row>
	<row>
		<field name="help_keyword_id">91</field>
		<field name="name">ROLLBACK</field>
	</row>
	<row>
		<field name="help_keyword_id">92</field>
		<field name="name">PROCEDURE</field>
	</row>
	<row>
		<field name="help_keyword_id">93</field>
		<field name="name">TIMESTAMP</field>
	</row>
	<row>
		<field name="help_keyword_id">94</field>
		<field name="name">IMPORT</field>
	</row>
	<row>
		<field name="help_keyword_id">95</field>
		<field name="name">AGAINST</field>
	</row>
	<row>
		<field name="help_keyword_id">96</field>
		<field name="name">CHECKSUM</field>
	</row>
	<row>
		<field name="help_keyword_id">97</field>
		<field name="name">COUNT</field>
	</row>
	<row>
		<field name="help_keyword_id">98</field>
		<field name="name">LONGBINARY</field>
	</row>
	<row>
		<field name="help_keyword_id">99</field>
		<field name="name">THEN</field>
	</row>
	<row>
		<field name="help_keyword_id">100</field>
		<field name="name">INSERT</field>
	</row>
	<row>
		<field name="help_keyword_id">101</field>
		<field name="name">ENGINES</field>
	</row>
	<row>
		<field name="help_keyword_id">102</field>
		<field name="name">HANDLER</field>
	</row>
	<row>
		<field name="help_keyword_id">103</field>
		<field name="name">PORT</field>
	</row>
	<row>
		<field name="help_keyword_id">104</field>
		<field name="name">DAY_SECOND</field>
	</row>
	<row>
		<field name="help_keyword_id">105</field>
		<field name="name">EXISTS</field>
	</row>
	<row>
		<field name="help_keyword_id">106</field>
		<field name="name">MUTEX</field>
	</row>
	<row>
		<field name="help_keyword_id">107</field>
		<field name="name">HELP_DATE</field>
	</row>
	<row>
		<field name="help_keyword_id">108</field>
		<field name="name">RELEASE</field>
	</row>
	<row>
		<field name="help_keyword_id">109</field>
		<field name="name">BOOLEAN</field>
	</row>
	<row>
		<field name="help_keyword_id">110</field>
		<field name="name">MOD</field>
	</row>
	<row>
		<field name="help_keyword_id">111</field>
		<field name="name">DEFAULT</field>
	</row>
	<row>
		<field name="help_keyword_id">112</field>
		<field name="name">TYPE</field>
	</row>
	<row>
		<field name="help_keyword_id">113</field>
		<field name="name">NO_WRITE_TO_BINLOG</field>
	</row>
	<row>
		<field name="help_keyword_id">114</field>
		<field name="name">OPTIMIZE</field>
	</row>
	<row>
		<field name="help_keyword_id">115</field>
		<field name="name">SQLSTATE</field>
	</row>
	<row>
		<field name="help_keyword_id">116</field>
		<field name="name">RESET</field>
	</row>
	<row>
		<field name="help_keyword_id">117</field>
		<field name="name">CLASS_ORIGIN</field>
	</row>
	<row>
		<field name="help_keyword_id">118</field>
		<field name="name">INSTALL</field>
	</row>
	<row>
		<field name="help_keyword_id">119</field>
		<field name="name">ITERATE</field>
	</row>
	<row>
		<field name="help_keyword_id">120</field>
		<field name="name">DO</field>
	</row>
	<row>
		<field name="help_keyword_id">121</field>
		<field name="name">BIGINT</field>
	</row>
	<row>
		<field name="help_keyword_id">122</field>
		<field name="name">SET</field>
	</row>
	<row>
		<field name="help_keyword_id">123</field>
		<field name="name">ISSUER</field>
	</row>
	<row>
		<field name="help_keyword_id">124</field>
		<field name="name">DATE</field>
	</row>
	<row>
		<field name="help_keyword_id">125</field>
		<field name="name">STATUS</field>
	</row>
	<row>
		<field name="help_keyword_id">126</field>
		<field name="name">FULLTEXT</field>
	</row>
	<row>
		<field name="help_keyword_id">127</field>
		<field name="name">COMMENT</field>
	</row>
	<row>
		<field name="help_keyword_id">128</field>
		<field name="name">MASTER_CONNECT_RETRY</field>
	</row>
	<row>
		<field name="help_keyword_id">129</field>
		<field name="name">INNER</field>
	</row>
	<row>
		<field name="help_keyword_id">130</field>
		<field name="name">RELAYLOG</field>
	</row>
	<row>
		<field name="help_keyword_id">131</field>
		<field name="name">STOP</field>
	</row>
	<row>
		<field name="help_keyword_id">132</field>
		<field name="name">MASTER_LOG_FILE</field>
	</row>
	<row>
		<field name="help_keyword_id">133</field>
		<field name="name">MRG_MYISAM</field>
	</row>
	<row>
		<field name="help_keyword_id">134</field>
		<field name="name">PRECISION</field>
	</row>
	<row>
		<field name="help_keyword_id">135</field>
		<field name="name">REQUIRE</field>
	</row>
	<row>
		<field name="help_keyword_id">136</field>
		<field name="name">TRAILING</field>
	</row>
	<row>
		<field name="help_keyword_id">137</field>
		<field name="name">PARTITIONS</field>
	</row>
	<row>
		<field name="help_keyword_id">138</field>
		<field name="name">LONG</field>
	</row>
	<row>
		<field name="help_keyword_id">139</field>
		<field name="name">OPTION</field>
	</row>
	<row>
		<field name="help_keyword_id">140</field>
		<field name="name">REORGANIZE</field>
	</row>
	<row>
		<field name="help_keyword_id">141</field>
		<field name="name">ELSE</field>
	</row>
	<row>
		<field name="help_keyword_id">142</field>
		<field name="name">DEALLOCATE</field>
	</row>
	<row>
		<field name="help_keyword_id">143</field>
		<field name="name">IO_THREAD</field>
	</row>
	<row>
		<field name="help_keyword_id">144</field>
		<field name="name">CASE</field>
	</row>
	<row>
		<field name="help_keyword_id">145</field>
		<field name="name">CIPHER</field>
	</row>
	<row>
		<field name="help_keyword_id">146</field>
		<field name="name">CONTINUE</field>
	</row>
	<row>
		<field name="help_keyword_id">147</field>
		<field name="name">FROM</field>
	</row>
	<row>
		<field name="help_keyword_id">148</field>
		<field name="name">READ</field>
	</row>
	<row>
		<field name="help_keyword_id">149</field>
		<field name="name">LEFT</field>
	</row>
	<row>
		<field name="help_keyword_id">150</field>
		<field name="name">ELSEIF</field>
	</row>
	<row>
		<field name="help_keyword_id">151</field>
		<field name="name">MINUTE_SECOND</field>
	</row>
	<row>
		<field name="help_keyword_id">152</field>
		<field name="name">COMPACT</field>
	</row>
	<row>
		<field name="help_keyword_id">153</field>
		<field name="name">DEC</field>
	</row>
	<row>
		<field name="help_keyword_id">154</field>
		<field name="name">FOR</field>
	</row>
	<row>
		<field name="help_keyword_id">155</field>
		<field name="name">WARNINGS</field>
	</row>
	<row>
		<field name="help_keyword_id">156</field>
		<field name="name">MIN_ROWS</field>
	</row>
	<row>
		<field name="help_keyword_id">157</field>
		<field name="name">STRING</field>
	</row>
	<row>
		<field name="help_keyword_id">158</field>
		<field name="name">CONDITION</field>
	</row>
	<row>
		<field name="help_keyword_id">159</field>
		<field name="name">ENCLOSED</field>
	</row>
	<row>
		<field name="help_keyword_id">160</field>
		<field name="name">FUNCTION</field>
	</row>
	<row>
		<field name="help_keyword_id">161</field>
		<field name="name">AGGREGATE</field>
	</row>
	<row>
		<field name="help_keyword_id">162</field>
		<field name="name">FIELDS</field>
	</row>
	<row>
		<field name="help_keyword_id">163</field>
		<field name="name">INT3</field>
	</row>
	<row>
		<field name="help_keyword_id">164</field>
		<field name="name">ARCHIVE</field>
	</row>
	<row>
		<field name="help_keyword_id">165</field>
		<field name="name">AVG_ROW_LENGTH</field>
	</row>
	<row>
		<field name="help_keyword_id">166</field>
		<field name="name">ADD</field>
	</row>
	<row>
		<field name="help_keyword_id">167</field>
		<field name="name">KILL</field>
	</row>
	<row>
		<field name="help_keyword_id">168</field>
		<field name="name">FLOAT4</field>
	</row>
	<row>
		<field name="help_keyword_id">169</field>
		<field name="name">TABLESPACE</field>
	</row>
	<row>
		<field name="help_keyword_id">170</field>
		<field name="name">VIEW</field>
	</row>
	<row>
		<field name="help_keyword_id">171</field>
		<field name="name">REPEATABLE</field>
	</row>
	<row>
		<field name="help_keyword_id">172</field>
		<field name="name">INFILE</field>
	</row>
	<row>
		<field name="help_keyword_id">173</field>
		<field name="name">HELP_VERSION</field>
	</row>
	<row>
		<field name="help_keyword_id">174</field>
		<field name="name">ORDER</field>
	</row>
	<row>
		<field name="help_keyword_id">175</field>
		<field name="name">USING</field>
	</row>
	<row>
		<field name="help_keyword_id">176</field>
		<field name="name">CONSTRAINT_CATALOG</field>
	</row>
	<row>
		<field name="help_keyword_id">177</field>
		<field name="name">MIDDLEINT</field>
	</row>
	<row>
		<field name="help_keyword_id">178</field>
		<field name="name">GRANT</field>
	</row>
	<row>
		<field name="help_keyword_id">179</field>
		<field name="name">UNSIGNED</field>
	</row>
	<row>
		<field name="help_keyword_id">180</field>
		<field name="name">DECIMAL</field>
	</row>
	<row>
		<field name="help_keyword_id">181</field>
		<field name="name">GEOMETRYFROMTEXT</field>
	</row>
	<row>
		<field name="help_keyword_id">182</field>
		<field name="name">INDEXES</field>
	</row>
	<row>
		<field name="help_keyword_id">183</field>
		<field name="name">FOREIGN</field>
	</row>
	<row>
		<field name="help_keyword_id">184</field>
		<field name="name">CACHE</field>
	</row>
	<row>
		<field name="help_keyword_id">185</field>
		<field name="name">HOSTS</field>
	</row>
	<row>
		<field name="help_keyword_id">186</field>
		<field name="name">MYSQL_ERRNO</field>
	</row>
	<row>
		<field name="help_keyword_id">187</field>
		<field name="name">COMMIT</field>
	</row>
	<row>
		<field name="help_keyword_id">188</field>
		<field name="name">SCHEMAS</field>
	</row>
	<row>
		<field name="help_keyword_id">189</field>
		<field name="name">LEADING</field>
	</row>
	<row>
		<field name="help_keyword_id">190</field>
		<field name="name">SNAPSHOT</field>
	</row>
	<row>
		<field name="help_keyword_id">191</field>
		<field name="name">CONSTRAINT_NAME</field>
	</row>
	<row>
		<field name="help_keyword_id">192</field>
		<field name="name">DECLARE</field>
	</row>
	<row>
		<field name="help_keyword_id">193</field>
		<field name="name">LOAD</field>
	</row>
	<row>
		<field name="help_keyword_id">194</field>
		<field name="name">SQL_CACHE</field>
	</row>
	<row>
		<field name="help_keyword_id">195</field>
		<field name="name">CONVERT</field>
	</row>
	<row>
		<field name="help_keyword_id">196</field>
		<field name="name">DYNAMIC</field>
	</row>
	<row>
		<field name="help_keyword_id">197</field>
		<field name="name">COLLATE</field>
	</row>
	<row>
		<field name="help_keyword_id">198</field>
		<field name="name">POLYGONFROMTEXT</field>
	</row>
	<row>
		<field name="help_keyword_id">199</field>
		<field name="name">BYTE</field>
	</row>
	<row>
		<field name="help_keyword_id">200</field>
		<field name="name">GLOBAL</field>
	</row>
	<row>
		<field name="help_keyword_id">201</field>
		<field name="name">LINESTRINGFROMWKB</field>
	</row>
	<row>
		<field name="help_keyword_id">202</field>
		<field name="name">WHEN</field>
	</row>
	<row>
		<field name="help_keyword_id">203</field>
		<field name="name">HAVING</field>
	</row>
	<row>
		<field name="help_keyword_id">204</field>
		<field name="name">AS</field>
	</row>
	<row>
		<field name="help_keyword_id">205</field>
		<field name="name">STARTING</field>
	</row>
	<row>
		<field name="help_keyword_id">206</field>
		<field name="name">RELOAD</field>
	</row>
	<row>
		<field name="help_keyword_id">207</field>
		<field name="name">AUTOCOMMIT</field>
	</row>
	<row>
		<field name="help_keyword_id">208</field>
		<field name="name">REVOKE</field>
	</row>
	<row>
		<field name="help_keyword_id">209</field>
		<field name="name">GRANTS</field>
	</row>
	<row>
		<field name="help_keyword_id">210</field>
		<field name="name">OUTER</field>
	</row>
	<row>
		<field name="help_keyword_id">211</field>
		<field name="name">CURSOR_NAME</field>
	</row>
	<row>
		<field name="help_keyword_id">212</field>
		<field name="name">FLOOR</field>
	</row>
	<row>
		<field name="help_keyword_id">213</field>
		<field name="name">EXPLAIN</field>
	</row>
	<row>
		<field name="help_keyword_id">214</field>
		<field name="name">WITH</field>
	</row>
	<row>
		<field name="help_keyword_id">215</field>
		<field name="name">AFTER</field>
	</row>
	<row>
		<field name="help_keyword_id">216</field>
		<field name="name">STD</field>
	</row>
	<row>
		<field name="help_keyword_id">217</field>
		<field name="name">CSV</field>
	</row>
	<row>
		<field name="help_keyword_id">218</field>
		<field name="name">DISABLE</field>
	</row>
	<row>
		<field name="help_keyword_id">219</field>
		<field name="name">UNINSTALL</field>
	</row>
	<row>
		<field name="help_keyword_id">220</field>
		<field name="name">OUTFILE</field>
	</row>
	<row>
		<field name="help_keyword_id">221</field>
		<field name="name">LOW_PRIORITY</field>
	</row>
	<row>
		<field name="help_keyword_id">222</field>
		<field name="name">FILE</field>
	</row>
	<row>
		<field name="help_keyword_id">223</field>
		<field name="name">NODEGROUP</field>
	</row>
	<row>
		<field name="help_keyword_id">224</field>
		<field name="name">SCHEMA</field>
	</row>
	<row>
		<field name="help_keyword_id">225</field>
		<field name="name">SONAME</field>
	</row>
	<row>
		<field name="help_keyword_id">226</field>
		<field name="name">POW</field>
	</row>
	<row>
		<field name="help_keyword_id">227</field>
		<field name="name">DUAL</field>
	</row>
	<row>
		<field name="help_keyword_id">228</field>
		<field name="name">MULTIPOINTFROMWKB</field>
	</row>
	<row>
		<field name="help_keyword_id">229</field>
		<field name="name">INDEX</field>
	</row>
	<row>
		<field name="help_keyword_id">230</field>
		<field name="name">MULTIPOINTFROMTEXT</field>
	</row>
	<row>
		<field name="help_keyword_id">231</field>
		<field name="name">DEFINER</field>
	</row>
	<row>
		<field name="help_keyword_id">232</field>
		<field name="name">MASTER_BIND</field>
	</row>
	<row>
		<field name="help_keyword_id">233</field>
		<field name="name">REMOVE</field>
	</row>
	<row>
		<field name="help_keyword_id">234</field>
		<field name="name">EXTENDED</field>
	</row>
	<row>
		<field name="help_keyword_id">235</field>
		<field name="name">MULTILINESTRINGFROMWKB</field>
	</row>
	<row>
		<field name="help_keyword_id">236</field>
		<field name="name">CROSS</field>
	</row>
	<row>
		<field name="help_keyword_id">237</field>
		<field name="name">CONTRIBUTORS</field>
	</row>
	<row>
		<field name="help_keyword_id">238</field>
		<field name="name">NATIONAL</field>
	</row>
	<row>
		<field name="help_keyword_id">239</field>
		<field name="name">GROUP</field>
	</row>
	<row>
		<field name="help_keyword_id">240</field>
		<field name="name">SHA</field>
	</row>
	<row>
		<field name="help_keyword_id">241</field>
		<field name="name">ONLINE</field>
	</row>
	<row>
		<field name="help_keyword_id">242</field>
		<field name="name">UNDO</field>
	</row>
	<row>
		<field name="help_keyword_id">243</field>
		<field name="name">IGNORE_SERVER_IDS</field>
	</row>
	<row>
		<field name="help_keyword_id">244</field>
		<field name="name">ZEROFILL</field>
	</row>
	<row>
		<field name="help_keyword_id">245</field>
		<field name="name">CLIENT</field>
	</row>
	<row>
		<field name="help_keyword_id">246</field>
		<field name="name">MASTER_PASSWORD</field>
	</row>
	<row>
		<field name="help_keyword_id">247</field>
		<field name="name">OWNER</field>
	</row>
	<row>
		<field name="help_keyword_id">248</field>
		<field name="name">RELAY_LOG_FILE</field>
	</row>
	<row>
		<field name="help_keyword_id">249</field>
		<field name="name">TRUE</field>
	</row>
	<row>
		<field name="help_keyword_id">250</field>
		<field name="name">CHARACTER</field>
	</row>
	<row>
		<field name="help_keyword_id">251</field>
		<field name="name">MASTER_USER</field>
	</row>
	<row>
		<field name="help_keyword_id">252</field>
		<field name="name">SCHEMA_NAME</field>
	</row>
	<row>
		<field name="help_keyword_id">253</field>
		<field name="name">TABLE</field>
	</row>
	<row>
		<field name="help_keyword_id">254</field>
		<field name="name">ENGINE</field>
	</row>
	<row>
		<field name="help_keyword_id">255</field>
		<field name="name">INSERT_METHOD</field>
	</row>
	<row>
		<field name="help_keyword_id">256</field>
		<field name="name">CASCADE</field>
	</row>
	<row>
		<field name="help_keyword_id">257</field>
		<field name="name">RELAY_LOG_POS</field>
	</row>
	<row>
		<field name="help_keyword_id">258</field>
		<field name="name">SQL_CALC_FOUND_ROWS</field>
	</row>
	<row>
		<field name="help_keyword_id">259</field>
		<field name="name">UNION</field>
	</row>
	<row>
		<field name="help_keyword_id">260</field>
		<field name="name">MYISAM</field>
	</row>
	<row>
		<field name="help_keyword_id">261</field>
		<field name="name">LEAVE</field>
	</row>
	<row>
		<field name="help_keyword_id">262</field>
		<field name="name">MODIFY</field>
	</row>
	<row>
		<field name="help_keyword_id">263</field>
		<field name="name">MATCH</field>
	</row>
	<row>
		<field name="help_keyword_id">264</field>
		<field name="name">MASTER_LOG_POS</field>
	</row>
	<row>
		<field name="help_keyword_id">265</field>
		<field name="name">DISTINCTROW</field>
	</row>
	<row>
		<field name="help_keyword_id">266</field>
		<field name="name">DESC</field>
	</row>
	<row>
		<field name="help_keyword_id">267</field>
		<field name="name">TIME</field>
	</row>
	<row>
		<field name="help_keyword_id">268</field>
		<field name="name">NUMERIC</field>
	</row>
	<row>
		<field name="help_keyword_id">269</field>
		<field name="name">EXPANSION</field>
	</row>
	<row>
		<field name="help_keyword_id">270</field>
		<field name="name">CODE</field>
	</row>
	<row>
		<field name="help_keyword_id">271</field>
		<field name="name">CURSOR</field>
	</row>
	<row>
		<field name="help_keyword_id">272</field>
		<field name="name">GEOMETRYCOLLECTIONFROMTEXT</field>
	</row>
	<row>
		<field name="help_keyword_id">273</field>
		<field name="name">CHAIN</field>
	</row>
	<row>
		<field name="help_keyword_id">274</field>
		<field name="name">LOGFILE</field>
	</row>
	<row>
		<field name="help_keyword_id">275</field>
		<field name="name">FLUSH</field>
	</row>
	<row>
		<field name="help_keyword_id">276</field>
		<field name="name">CREATE</field>
	</row>
	<row>
		<field name="help_keyword_id">277</field>
		<field name="name">DESCRIBE</field>
	</row>
	<row>
		<field name="help_keyword_id">278</field>
		<field name="name">EXTENT_SIZE</field>
	</row>
	<row>
		<field name="help_keyword_id">279</field>
		<field name="name">MAX_UPDATES_PER_HOUR</field>
	</row>
	<row>
		<field name="help_keyword_id">280</field>
		<field name="name">INT2</field>
	</row>
	<row>
		<field name="help_keyword_id">281</field>
		<field name="name">PROCESSLIST</field>
	</row>
	<row>
		<field name="help_keyword_id">282</field>
		<field name="name">ENDS</field>
	</row>
	<row>
		<field name="help_keyword_id">283</field>
		<field name="name">LOGS</field>
	</row>
	<row>
		<field name="help_keyword_id">284</field>
		<field name="name">RECOVER</field>
	</row>
	<row>
		<field name="help_keyword_id">285</field>
		<field name="name">DISCARD</field>
	</row>
	<row>
		<field name="help_keyword_id">286</field>
		<field name="name">HEAP</field>
	</row>
	<row>
		<field name="help_keyword_id">287</field>
		<field name="name">SOUNDS</field>
	</row>
	<row>
		<field name="help_keyword_id">288</field>
		<field name="name">BETWEEN</field>
	</row>
	<row>
		<field name="help_keyword_id">289</field>
		<field name="name">MULTILINESTRINGFROMTEXT</field>
	</row>
	<row>
		<field name="help_keyword_id">290</field>
		<field name="name">REPAIR</field>
	</row>
	<row>
		<field name="help_keyword_id">291</field>
		<field name="name">PACK_KEYS</field>
	</row>
	<row>
		<field name="help_keyword_id">292</field>
		<field name="name">FAST</field>
	</row>
	<row>
		<field name="help_keyword_id">293</field>
		<field name="name">VALUES</field>
	</row>
	<row>
		<field name="help_keyword_id">294</field>
		<field name="name">CALL</field>
	</row>
	<row>
		<field name="help_keyword_id">295</field>
		<field name="name">LOOP</field>
	</row>
	<row>
		<field name="help_keyword_id">296</field>
		<field name="name">VARCHARACTER</field>
	</row>
	<row>
		<field name="help_keyword_id">297</field>
		<field name="name">BEFORE</field>
	</row>
	<row>
		<field name="help_keyword_id">298</field>
		<field name="name">TRUNCATE</field>
	</row>
	<row>
		<field name="help_keyword_id">299</field>
		<field name="name">SHOW</field>
	</row>
	<row>
		<field name="help_keyword_id">300</field>
		<field name="name">ALL</field>
	</row>
	<row>
		<field name="help_keyword_id">301</field>
		<field name="name">REDUNDANT</field>
	</row>
	<row>
		<field name="help_keyword_id">302</field>
		<field name="name">USER_RESOURCES</field>
	</row>
	<row>
		<field name="help_keyword_id">303</field>
		<field name="name">PARTIAL</field>
	</row>
	<row>
		<field name="help_keyword_id">304</field>
		<field name="name">BINLOG</field>
	</row>
	<row>
		<field name="help_keyword_id">305</field>
		<field name="name">END</field>
	</row>
	<row>
		<field name="help_keyword_id">306</field>
		<field name="name">SECOND</field>
	</row>
	<row>
		<field name="help_keyword_id">307</field>
		<field name="name">AND</field>
	</row>
	<row>
		<field name="help_keyword_id">308</field>
		<field name="name">FLOAT8</field>
	</row>
	<row>
		<field name="help_keyword_id">309</field>
		<field name="name">PREV</field>
	</row>
	<row>
		<field name="help_keyword_id">310</field>
		<field name="name">HOUR</field>
	</row>
	<row>
		<field name="help_keyword_id">311</field>
		<field name="name">SELECT</field>
	</row>
	<row>
		<field name="help_keyword_id">312</field>
		<field name="name">DATABASES</field>
	</row>
	<row>
		<field name="help_keyword_id">313</field>
		<field name="name">OR</field>
	</row>
	<row>
		<field name="help_keyword_id">314</field>
		<field name="name">IDENTIFIED</field>
	</row>
	<row>
		<field name="help_keyword_id">315</field>
		<field name="name">WRAPPER</field>
	</row>
	<row>
		<field name="help_keyword_id">316</field>
		<field name="name">MASTER_SSL_CIPHER</field>
	</row>
	<row>
		<field name="help_keyword_id">317</field>
		<field name="name">SQL_SLAVE_SKIP_COUNTER</field>
	</row>
	<row>
		<field name="help_keyword_id">318</field>
		<field name="name">BOTH</field>
	</row>
	<row>
		<field name="help_keyword_id">319</field>
		<field name="name">BOOL</field>
	</row>
	<row>
		<field name="help_keyword_id">320</field>
		<field name="name">YEAR</field>
	</row>
	<row>
		<field name="help_keyword_id">321</field>
		<field name="name">MASTER_PORT</field>
	</row>
	<row>
		<field name="help_keyword_id">322</field>
		<field name="name">CONCURRENT</field>
	</row>
	<row>
		<field name="help_keyword_id">323</field>
		<field name="name">HELP</field>
	</row>
	<row>
		<field name="help_keyword_id">324</field>
		<field name="name">UNIQUE</field>
	</row>
	<row>
		<field name="help_keyword_id">325</field>
		<field name="name">TRIGGERS</field>
	</row>
	<row>
		<field name="help_keyword_id">326</field>
		<field name="name">PROCESS</field>
	</row>
	<row>
		<field name="help_keyword_id">327</field>
		<field name="name">OPTIONS</field>
	</row>
	<row>
		<field name="help_keyword_id">328</field>
		<field name="name">RESIGNAL</field>
	</row>
	<row>
		<field name="help_keyword_id">329</field>
		<field name="name">CONSISTENT</field>
	</row>
	<row>
		<field name="help_keyword_id">330</field>
		<field name="name">MASTER_SSL</field>
	</row>
	<row>
		<field name="help_keyword_id">331</field>
		<field name="name">DATE_ADD</field>
	</row>
	<row>
		<field name="help_keyword_id">332</field>
		<field name="name">MAX_CONNECTIONS_PER_HOUR</field>
	</row>
	<row>
		<field name="help_keyword_id">333</field>
		<field name="name">LIKE</field>
	</row>
	<row>
		<field name="help_keyword_id">334</field>
		<field name="name">PLUGIN</field>
	</row>
	<row>
		<field name="help_keyword_id">335</field>
		<field name="name">FETCH</field>
	</row>
	<row>
		<field name="help_keyword_id">336</field>
		<field name="name">IN</field>
	</row>
	<row>
		<field name="help_keyword_id">337</field>
		<field name="name">COLUMN</field>
	</row>
	<row>
		<field name="help_keyword_id">338</field>
		<field name="name">DUMPFILE</field>
	</row>
	<row>
		<field name="help_keyword_id">339</field>
		<field name="name">USAGE</field>
	</row>
	<row>
		<field name="help_keyword_id">340</field>
		<field name="name">EXECUTE</field>
	</row>
	<row>
		<field name="help_keyword_id">341</field>
		<field name="name">MEMORY</field>
	</row>
	<row>
		<field name="help_keyword_id">342</field>
		<field name="name">CEIL</field>
	</row>
	<row>
		<field name="help_keyword_id">343</field>
		<field name="name">QUERY</field>
	</row>
	<row>
		<field name="help_keyword_id">344</field>
		<field name="name">MASTER_HOST</field>
	</row>
	<row>
		<field name="help_keyword_id">345</field>
		<field name="name">LINES</field>
	</row>
	<row>
		<field name="help_keyword_id">346</field>
		<field name="name">SQL_THREAD</field>
	</row>
	<row>
		<field name="help_keyword_id">347</field>
		<field name="name">SERVER</field>
	</row>
	<row>
		<field name="help_keyword_id">348</field>
		<field name="name">MAX_QUERIES_PER_HOUR</field>
	</row>
	<row>
		<field name="help_keyword_id">349</field>
		<field name="name">MASTER_SSL_CERT</field>
	</row>
	<row>
		<field name="help_keyword_id">350</field>
		<field name="name">MULTIPOLYGONFROMWKB</field>
	</row>
	<row>
		<field name="help_keyword_id">351</field>
		<field name="name">TRANSACTION</field>
	</row>
	<row>
		<field name="help_keyword_id">352</field>
		<field name="name">DAY_MINUTE</field>
	</row>
	<row>
		<field name="help_keyword_id">353</field>
		<field name="name">STDDEV</field>
	</row>
	<row>
		<field name="help_keyword_id">354</field>
		<field name="name">DATE_SUB</field>
	</row>
	<row>
		<field name="help_keyword_id">355</field>
		<field name="name">REBUILD</field>
	</row>
	<row>
		<field name="help_keyword_id">356</field>
		<field name="name">GEOMETRYFROMWKB</field>
	</row>
	<row>
		<field name="help_keyword_id">357</field>
		<field name="name">INT1</field>
	</row>
	<row>
		<field name="help_keyword_id">358</field>
		<field name="name">RENAME</field>
	</row>
	<row>
		<field name="help_keyword_id">359</field>
		<field name="name">PARSER</field>
	</row>
	<row>
		<field name="help_keyword_id">360</field>
		<field name="name">RIGHT</field>
	</row>
	<row>
		<field name="help_keyword_id">361</field>
		<field name="name">ALTER</field>
	</row>
	<row>
		<field name="help_keyword_id">362</field>
		<field name="name">MAX_ROWS</field>
	</row>
	<row>
		<field name="help_keyword_id">363</field>
		<field name="name">SOCKET</field>
	</row>
	<row>
		<field name="help_keyword_id">364</field>
		<field name="name">STRAIGHT_JOIN</field>
	</row>
	<row>
		<field name="help_keyword_id">365</field>
		<field name="name">NATURAL</field>
	</row>
	<row>
		<field name="help_keyword_id">366</field>
		<field name="name">VARIABLES</field>
	</row>
	<row>
		<field name="help_keyword_id">367</field>
		<field name="name">ESCAPED</field>
	</row>
	<row>
		<field name="help_keyword_id">368</field>
		<field name="name">SHA1</field>
	</row>
	<row>
		<field name="help_keyword_id">369</field>
		<field name="name">KEY_BLOCK_SIZE</field>
	</row>
	<row>
		<field name="help_keyword_id">370</field>
		<field name="name">PASSWORD</field>
	</row>
	<row>
		<field name="help_keyword_id">371</field>
		<field name="name">OFFSET</field>
	</row>
	<row>
		<field name="help_keyword_id">372</field>
		<field name="name">CHAR</field>
	</row>
	<row>
		<field name="help_keyword_id">373</field>
		<field name="name">NEXT</field>
	</row>
	<row>
		<field name="help_keyword_id">374</field>
		<field name="name">ERRORS</field>
	</row>
	<row>
		<field name="help_keyword_id">375</field>
		<field name="name">SQL_LOG_BIN</field>
	</row>
	<row>
		<field name="help_keyword_id">376</field>
		<field name="name">TEMPORARY</field>
	</row>
	<row>
		<field name="help_keyword_id">377</field>
		<field name="name">COMMITTED</field>
	</row>
	<row>
		<field name="help_keyword_id">378</field>
		<field name="name">SQL_SMALL_RESULT</field>
	</row>
	<row>
		<field name="help_keyword_id">379</field>
		<field name="name">UPGRADE</field>
	</row>
	<row>
		<field name="help_keyword_id">380</field>
		<field name="name">XA</field>
	</row>
	<row>
		<field name="help_keyword_id">381</field>
		<field name="name">BEGIN</field>
	</row>
	<row>
		<field name="help_keyword_id">382</field>
		<field name="name">DELAY_KEY_WRITE</field>
	</row>
	<row>
		<field name="help_keyword_id">383</field>
		<field name="name">PROFILE</field>
	</row>
	<row>
		<field name="help_keyword_id">384</field>
		<field name="name">MEDIUM</field>
	</row>
	<row>
		<field name="help_keyword_id">385</field>
		<field name="name">INTERVAL</field>
	</row>
	<row>
		<field name="help_keyword_id">386</field>
		<field name="name">SSL</field>
	</row>
	<row>
		<field name="help_keyword_id">387</field>
		<field name="name">DAY_HOUR</field>
	</row>
	<row>
		<field name="help_keyword_id">388</field>
		<field name="name">NAME</field>
	</row>
	<row>
		<field name="help_keyword_id">389</field>
		<field name="name">REFERENCES</field>
	</row>
	<row>
		<field name="help_keyword_id">390</field>
		<field name="name">AES_ENCRYPT</field>
	</row>
	<row>
		<field name="help_keyword_id">391</field>
		<field name="name">STORAGE</field>
	</row>
	<row>
		<field name="help_keyword_id">392</field>
		<field name="name">ISOLATION</field>
	</row>
	<row>
		<field name="help_keyword_id">393</field>
		<field name="name">CEILING</field>
	</row>
	<row>
		<field name="help_keyword_id">394</field>
		<field name="name">EVERY</field>
	</row>
	<row>
		<field name="help_keyword_id">395</field>
		<field name="name">INT8</field>
	</row>
	<row>
		<field name="help_keyword_id">396</field>
		<field name="name">AUTHORS</field>
	</row>
	<row>
		<field name="help_keyword_id">397</field>
		<field name="name">RESTRICT</field>
	</row>
	<row>
		<field name="help_keyword_id">398</field>
		<field name="name">UNCOMMITTED</field>
	</row>
	<row>
		<field name="help_keyword_id">399</field>
		<field name="name">LINESTRINGFROMTEXT</field>
	</row>
	<row>
		<field name="help_keyword_id">400</field>
		<field name="name">IS</field>
	</row>
	<row>
		<field name="help_keyword_id">401</field>
		<field name="name">NOT</field>
	</row>
	<row>
		<field name="help_keyword_id">402</field>
		<field name="name">ANALYSE</field>
	</row>
	<row>
		<field name="help_keyword_id">403</field>
		<field name="name">DATAFILE</field>
	</row>
	<row>
		<field name="help_keyword_id">404</field>
		<field name="name">DES_KEY_FILE</field>
	</row>
	<row>
		<field name="help_keyword_id">405</field>
		<field name="name">SIGNAL</field>
	</row>
	<row>
		<field name="help_keyword_id">406</field>
		<field name="name">COMPRESSED</field>
	</row>
	<row>
		<field name="help_keyword_id">407</field>
		<field name="name">START</field>
	</row>
	<row>
		<field name="help_keyword_id">408</field>
		<field name="name">PLUGINS</field>
	</row>
	<row>
		<field name="help_keyword_id">409</field>
		<field name="name">SAVEPOINT</field>
	</row>
	<row>
		<field name="help_keyword_id">410</field>
		<field name="name">IF</field>
	</row>
	<row>
		<field name="help_keyword_id">411</field>
		<field name="name">ROWS</field>
	</row>
	<row>
		<field name="help_keyword_id">412</field>
		<field name="name">PRIMARY</field>
	</row>
	<row>
		<field name="help_keyword_id">413</field>
		<field name="name">PURGE</field>
	</row>
	<row>
		<field name="help_keyword_id">414</field>
		<field name="name">LAST</field>
	</row>
	<row>
		<field name="help_keyword_id">415</field>
		<field name="name">USER</field>
	</row>
	<row>
		<field name="help_keyword_id">416</field>
		<field name="name">EXIT</field>
	</row>
	<row>
		<field name="help_keyword_id">417</field>
		<field name="name">KEYS</field>
	</row>
	<row>
		<field name="help_keyword_id">418</field>
		<field name="name">LIMIT</field>
	</row>
	<row>
		<field name="help_keyword_id">419</field>
		<field name="name">KEY</field>
	</row>
	<row>
		<field name="help_keyword_id">420</field>
		<field name="name">MERGE</field>
	</row>
	<row>
		<field name="help_keyword_id">421</field>
		<field name="name">UNTIL</field>
	</row>
	<row>
		<field name="help_keyword_id">422</field>
		<field name="name">SQL_NO_CACHE</field>
	</row>
	<row>
		<field name="help_keyword_id">423</field>
		<field name="name">DELAYED</field>
	</row>
	<row>
		<field name="help_keyword_id">424</field>
		<field name="name">CONSTRAINT_SCHEMA</field>
	</row>
	<row>
		<field name="help_keyword_id">425</field>
		<field name="name">ANALYZE</field>
	</row>
	<row>
		<field name="help_keyword_id">426</field>
		<field name="name">CONSTRAINT</field>
	</row>
	<row>
		<field name="help_keyword_id">427</field>
		<field name="name">SERIAL</field>
	</row>
	<row>
		<field name="help_keyword_id">428</field>
		<field name="name">ACTION</field>
	</row>
	<row>
		<field name="help_keyword_id">429</field>
		<field name="name">WRITE</field>
	</row>
	<row>
		<field name="help_keyword_id">430</field>
		<field name="name">INITIAL_SIZE</field>
	</row>
	<row>
		<field name="help_keyword_id">431</field>
		<field name="name">SESSION</field>
	</row>
	<row>
		<field name="help_keyword_id">432</field>
		<field name="name">DATABASE</field>
	</row>
	<row>
		<field name="help_keyword_id">433</field>
		<field name="name">NULL</field>
	</row>
	<row>
		<field name="help_keyword_id">434</field>
		<field name="name">POWER</field>
	</row>
	<row>
		<field name="help_keyword_id">435</field>
		<field name="name">USE_FRM</field>
	</row>
	<row>
		<field name="help_keyword_id">436</field>
		<field name="name">TERMINATED</field>
	</row>
	<row>
		<field name="help_keyword_id">437</field>
		<field name="name">SLAVE</field>
	</row>
	<row>
		<field name="help_keyword_id">438</field>
		<field name="name">NVARCHAR</field>
	</row>
	<row>
		<field name="help_keyword_id">439</field>
		<field name="name">ASC</field>
	</row>
	<row>
		<field name="help_keyword_id">440</field>
		<field name="name">RETURN</field>
	</row>
	<row>
		<field name="help_keyword_id">441</field>
		<field name="name">OPTIONALLY</field>
	</row>
	<row>
		<field name="help_keyword_id">442</field>
		<field name="name">ENABLE</field>
	</row>
	<row>
		<field name="help_keyword_id">443</field>
		<field name="name">DIRECTORY</field>
	</row>
	<row>
		<field name="help_keyword_id">444</field>
		<field name="name">MAX_USER_CONNECTIONS</field>
	</row>
	<row>
		<field name="help_keyword_id">445</field>
		<field name="name">WHILE</field>
	</row>
	<row>
		<field name="help_keyword_id">446</field>
		<field name="name">LOCAL</field>
	</row>
	<row>
		<field name="help_keyword_id">447</field>
		<field name="name">DISTINCT</field>
	</row>
	<row>
		<field name="help_keyword_id">448</field>
		<field name="name">AES_DECRYPT</field>
	</row>
	<row>
		<field name="help_keyword_id">449</field>
		<field name="name">MASTER_SSL_KEY</field>
	</row>
	<row>
		<field name="help_keyword_id">450</field>
		<field name="name">NONE</field>
	</row>
	<row>
		<field name="help_keyword_id">451</field>
		<field name="name">TABLES</field>
	</row>
	<row>
		<field name="help_keyword_id">452</field>
		<field name="name">&lt;&gt;</field>
	</row>
	<row>
		<field name="help_keyword_id">453</field>
		<field name="name">RLIKE</field>
	</row>
	<row>
		<field name="help_keyword_id">454</field>
		<field name="name">TRIGGER</field>
	</row>
	<row>
		<field name="help_keyword_id">455</field>
		<field name="name">COLLATION</field>
	</row>
	<row>
		<field name="help_keyword_id">456</field>
		<field name="name">SHUTDOWN</field>
	</row>
	<row>
		<field name="help_keyword_id">457</field>
		<field name="name">HIGH_PRIORITY</field>
	</row>
	<row>
		<field name="help_keyword_id">458</field>
		<field name="name">BTREE</field>
	</row>
	<row>
		<field name="help_keyword_id">459</field>
		<field name="name">FIRST</field>
	</row>
	<row>
		<field name="help_keyword_id">460</field>
		<field name="name">COALESCE</field>
	</row>
	<row>
		<field name="help_keyword_id">461</field>
		<field name="name">WAIT</field>
	</row>
	<row>
		<field name="help_keyword_id">462</field>
		<field name="name">CATALOG_NAME</field>
	</row>
	<row>
		<field name="help_keyword_id">463</field>
		<field name="name">MASTER</field>
	</row>
	<row>
		<field name="help_keyword_id">464</field>
		<field name="name">FIXED</field>
	</row>
	<row>
		<field name="help_keyword_id">465</field>
		<field name="name">MULTIPOLYGONFROMTEXT</field>
	</row>
	<row>
		<field name="help_keyword_id">466</field>
		<field name="name">ROW_FORMAT</field>
	</row>
	</table_data>
	<table_structure name="help_relation">
		<field Field="help_topic_id" Type="int(10) unsigned" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="help_keyword_id" Type="int(10) unsigned" Null="NO" Key="PRI" Extra="" Comment="" />
		<key Table="help_relation" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="help_keyword_id" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="help_relation" Non_unique="0" Key_name="PRIMARY" Seq_in_index="2" Column_name="help_topic_id" Collation="A" Cardinality="1048" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="help_relation" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="1048" Avg_row_length="9" Data_length="9432" Max_data_length="2533274790395903" Index_length="18432" Data_free="0" Create_time="2013-06-14 00:06:54" Update_time="2013-06-14 00:06:56" Collation="utf8_general_ci" Create_options="" Comment="keyword-topic relation" />
	</table_structure>
	<table_data name="help_relation">
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">0</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">0</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">1</field>
	</row>
	<row>
		<field name="help_topic_id">234</field>
		<field name="help_keyword_id">2</field>
	</row>
	<row>
		<field name="help_topic_id">449</field>
		<field name="help_keyword_id">3</field>
	</row>
	<row>
		<field name="help_topic_id">3</field>
		<field name="help_keyword_id">4</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="help_keyword_id">4</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">4</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">5</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">6</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">6</field>
	</row>
	<row>
		<field name="help_topic_id">98</field>
		<field name="help_keyword_id">7</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">8</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">9</field>
	</row>
	<row>
		<field name="help_topic_id">431</field>
		<field name="help_keyword_id">10</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">11</field>
	</row>
	<row>
		<field name="help_topic_id">21</field>
		<field name="help_keyword_id">12</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">12</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">12</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">12</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">12</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">13</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">13</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="help_keyword_id">14</field>
	</row>
	<row>
		<field name="help_topic_id">231</field>
		<field name="help_keyword_id">15</field>
	</row>
	<row>
		<field name="help_topic_id">89</field>
		<field name="help_keyword_id">16</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">16</field>
	</row>
	<row>
		<field name="help_topic_id">18</field>
		<field name="help_keyword_id">17</field>
	</row>
	<row>
		<field name="help_topic_id">107</field>
		<field name="help_keyword_id">17</field>
	</row>
	<row>
		<field name="help_topic_id">135</field>
		<field name="help_keyword_id">17</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">17</field>
	</row>
	<row>
		<field name="help_topic_id">98</field>
		<field name="help_keyword_id">18</field>
	</row>
	<row>
		<field name="help_topic_id">502</field>
		<field name="help_keyword_id">18</field>
	</row>
	<row>
		<field name="help_topic_id">381</field>
		<field name="help_keyword_id">19</field>
	</row>
	<row>
		<field name="help_topic_id">3</field>
		<field name="help_keyword_id">20</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">20</field>
	</row>
	<row>
		<field name="help_topic_id">245</field>
		<field name="help_keyword_id">20</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">20</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">20</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">21</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">22</field>
	</row>
	<row>
		<field name="help_topic_id">9</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">30</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">34</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">88</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">187</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">232</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">239</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">265</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">278</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">302</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">332</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">406</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">416</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">417</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">480</field>
		<field name="help_keyword_id">23</field>
	</row>
	<row>
		<field name="help_topic_id">109</field>
		<field name="help_keyword_id">24</field>
	</row>
	<row>
		<field name="help_topic_id">123</field>
		<field name="help_keyword_id">25</field>
	</row>
	<row>
		<field name="help_topic_id">171</field>
		<field name="help_keyword_id">25</field>
	</row>
	<row>
		<field name="help_topic_id">425</field>
		<field name="help_keyword_id">25</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">26</field>
	</row>
	<row>
		<field name="help_topic_id">83</field>
		<field name="help_keyword_id">27</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">28</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">29</field>
	</row>
	<row>
		<field name="help_topic_id">36</field>
		<field name="help_keyword_id">30</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">31</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">31</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">32</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">33</field>
	</row>
	<row>
		<field name="help_topic_id">35</field>
		<field name="help_keyword_id">34</field>
	</row>
	<row>
		<field name="help_topic_id">239</field>
		<field name="help_keyword_id">34</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">34</field>
	</row>
	<row>
		<field name="help_topic_id">36</field>
		<field name="help_keyword_id">35</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">35</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">36</field>
	</row>
	<row>
		<field name="help_topic_id">418</field>
		<field name="help_keyword_id">37</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">37</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">37</field>
	</row>
	<row>
		<field name="help_topic_id">21</field>
		<field name="help_keyword_id">38</field>
	</row>
	<row>
		<field name="help_topic_id">296</field>
		<field name="help_keyword_id">38</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">38</field>
	</row>
	<row>
		<field name="help_topic_id">453</field>
		<field name="help_keyword_id">38</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">38</field>
	</row>
	<row>
		<field name="help_topic_id">502</field>
		<field name="help_keyword_id">39</field>
	</row>
	<row>
		<field name="help_topic_id">49</field>
		<field name="help_keyword_id">40</field>
	</row>
	<row>
		<field name="help_topic_id">78</field>
		<field name="help_keyword_id">40</field>
	</row>
	<row>
		<field name="help_topic_id">84</field>
		<field name="help_keyword_id">40</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="help_keyword_id">40</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">40</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">40</field>
	</row>
	<row>
		<field name="help_topic_id">362</field>
		<field name="help_keyword_id">40</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">40</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">40</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">40</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">41</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="help_keyword_id">41</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">42</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">43</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">43</field>
	</row>
	<row>
		<field name="help_topic_id">213</field>
		<field name="help_keyword_id">44</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">44</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">44</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">44</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">45</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">46</field>
	</row>
	<row>
		<field name="help_topic_id">98</field>
		<field name="help_keyword_id">47</field>
	</row>
	<row>
		<field name="help_topic_id">316</field>
		<field name="help_keyword_id">47</field>
	</row>
	<row>
		<field name="help_topic_id">362</field>
		<field name="help_keyword_id">48</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">49</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">49</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">50</field>
	</row>
	<row>
		<field name="help_topic_id">49</field>
		<field name="help_keyword_id">51</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">51</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="help_keyword_id">51</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">52</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">52</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">52</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="help_keyword_id">52</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">53</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">53</field>
	</row>
	<row>
		<field name="help_topic_id">177</field>
		<field name="help_keyword_id">54</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">54</field>
	</row>
	<row>
		<field name="help_topic_id">52</field>
		<field name="help_keyword_id">55</field>
	</row>
	<row>
		<field name="help_topic_id">107</field>
		<field name="help_keyword_id">55</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">56</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">57</field>
	</row>
	<row>
		<field name="help_topic_id">56</field>
		<field name="help_keyword_id">57</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">57</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">58</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">58</field>
	</row>
	<row>
		<field name="help_topic_id">49</field>
		<field name="help_keyword_id">59</field>
	</row>
	<row>
		<field name="help_topic_id">84</field>
		<field name="help_keyword_id">59</field>
	</row>
	<row>
		<field name="help_topic_id">107</field>
		<field name="help_keyword_id">59</field>
	</row>
	<row>
		<field name="help_topic_id">194</field>
		<field name="help_keyword_id">60</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">60</field>
	</row>
	<row>
		<field name="help_topic_id">251</field>
		<field name="help_keyword_id">60</field>
	</row>
	<row>
		<field name="help_topic_id">211</field>
		<field name="help_keyword_id">61</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">61</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">62</field>
	</row>
	<row>
		<field name="help_topic_id">302</field>
		<field name="help_keyword_id">62</field>
	</row>
	<row>
		<field name="help_topic_id">359</field>
		<field name="help_keyword_id">62</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">62</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">63</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">64</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">65</field>
	</row>
	<row>
		<field name="help_topic_id">84</field>
		<field name="help_keyword_id">65</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">65</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="help_keyword_id">65</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">65</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">65</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">65</field>
	</row>
	<row>
		<field name="help_topic_id">67</field>
		<field name="help_keyword_id">66</field>
	</row>
	<row>
		<field name="help_topic_id">49</field>
		<field name="help_keyword_id">67</field>
	</row>
	<row>
		<field name="help_topic_id">418</field>
		<field name="help_keyword_id">67</field>
	</row>
	<row>
		<field name="help_topic_id">468</field>
		<field name="help_keyword_id">67</field>
	</row>
	<row>
		<field name="help_topic_id">231</field>
		<field name="help_keyword_id">68</field>
	</row>
	<row>
		<field name="help_topic_id">88</field>
		<field name="help_keyword_id">69</field>
	</row>
	<row>
		<field name="help_topic_id">211</field>
		<field name="help_keyword_id">69</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">69</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">70</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">71</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">72</field>
	</row>
	<row>
		<field name="help_topic_id">92</field>
		<field name="help_keyword_id">73</field>
	</row>
	<row>
		<field name="help_topic_id">484</field>
		<field name="help_keyword_id">73</field>
	</row>
	<row>
		<field name="help_topic_id">386</field>
		<field name="help_keyword_id">74</field>
	</row>
	<row>
		<field name="help_topic_id">449</field>
		<field name="help_keyword_id">75</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">76</field>
	</row>
	<row>
		<field name="help_topic_id">39</field>
		<field name="help_keyword_id">77</field>
	</row>
	<row>
		<field name="help_topic_id">231</field>
		<field name="help_keyword_id">77</field>
	</row>
	<row>
		<field name="help_topic_id">271</field>
		<field name="help_keyword_id">77</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">78</field>
	</row>
	<row>
		<field name="help_topic_id">271</field>
		<field name="help_keyword_id">78</field>
	</row>
	<row>
		<field name="help_topic_id">462</field>
		<field name="help_keyword_id">78</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">79</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">79</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">80</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">80</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">81</field>
	</row>
	<row>
		<field name="help_topic_id">84</field>
		<field name="help_keyword_id">82</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">82</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">82</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="help_keyword_id">82</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">83</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">83</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">84</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">84</field>
	</row>
	<row>
		<field name="help_topic_id">3</field>
		<field name="help_keyword_id">85</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">85</field>
	</row>
	<row>
		<field name="help_topic_id">306</field>
		<field name="help_keyword_id">85</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">85</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">86</field>
	</row>
	<row>
		<field name="help_topic_id">259</field>
		<field name="help_keyword_id">87</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">88</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">89</field>
	</row>
	<row>
		<field name="help_topic_id">130</field>
		<field name="help_keyword_id">90</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="help_keyword_id">91</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">91</field>
	</row>
	<row>
		<field name="help_topic_id">462</field>
		<field name="help_keyword_id">91</field>
	</row>
	<row>
		<field name="help_topic_id">17</field>
		<field name="help_keyword_id">92</field>
	</row>
	<row>
		<field name="help_topic_id">190</field>
		<field name="help_keyword_id">92</field>
	</row>
	<row>
		<field name="help_topic_id">304</field>
		<field name="help_keyword_id">92</field>
	</row>
	<row>
		<field name="help_topic_id">330</field>
		<field name="help_keyword_id">92</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">92</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">92</field>
	</row>
	<row>
		<field name="help_topic_id">417</field>
		<field name="help_keyword_id">92</field>
	</row>
	<row>
		<field name="help_topic_id">441</field>
		<field name="help_keyword_id">92</field>
	</row>
	<row>
		<field name="help_topic_id">490</field>
		<field name="help_keyword_id">92</field>
	</row>
	<row>
		<field name="help_topic_id">100</field>
		<field name="help_keyword_id">93</field>
	</row>
	<row>
		<field name="help_topic_id">189</field>
		<field name="help_keyword_id">93</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">94</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">94</field>
	</row>
	<row>
		<field name="help_topic_id">89</field>
		<field name="help_keyword_id">95</field>
	</row>
	<row>
		<field name="help_topic_id">402</field>
		<field name="help_keyword_id">96</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">96</field>
	</row>
	<row>
		<field name="help_topic_id">70</field>
		<field name="help_keyword_id">97</field>
	</row>
	<row>
		<field name="help_topic_id">331</field>
		<field name="help_keyword_id">97</field>
	</row>
	<row>
		<field name="help_topic_id">434</field>
		<field name="help_keyword_id">97</field>
	</row>
	<row>
		<field name="help_topic_id">287</field>
		<field name="help_keyword_id">98</field>
	</row>
	<row>
		<field name="help_topic_id">28</field>
		<field name="help_keyword_id">99</field>
	</row>
	<row>
		<field name="help_topic_id">58</field>
		<field name="help_keyword_id">99</field>
	</row>
	<row>
		<field name="help_topic_id">86</field>
		<field name="help_keyword_id">99</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">100</field>
	</row>
	<row>
		<field name="help_topic_id">196</field>
		<field name="help_keyword_id">100</field>
	</row>
	<row>
		<field name="help_topic_id">303</field>
		<field name="help_keyword_id">100</field>
	</row>
	<row>
		<field name="help_topic_id">488</field>
		<field name="help_keyword_id">100</field>
	</row>
	<row>
		<field name="help_topic_id">284</field>
		<field name="help_keyword_id">101</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">101</field>
	</row>
	<row>
		<field name="help_topic_id">107</field>
		<field name="help_keyword_id">102</field>
	</row>
	<row>
		<field name="help_topic_id">315</field>
		<field name="help_keyword_id">102</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">103</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">104</field>
	</row>
	<row>
		<field name="help_topic_id">9</field>
		<field name="help_keyword_id">105</field>
	</row>
	<row>
		<field name="help_topic_id">30</field>
		<field name="help_keyword_id">105</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">105</field>
	</row>
	<row>
		<field name="help_topic_id">156</field>
		<field name="help_keyword_id">105</field>
	</row>
	<row>
		<field name="help_topic_id">187</field>
		<field name="help_keyword_id">105</field>
	</row>
	<row>
		<field name="help_topic_id">278</field>
		<field name="help_keyword_id">105</field>
	</row>
	<row>
		<field name="help_topic_id">302</field>
		<field name="help_keyword_id">105</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">106</field>
	</row>
	<row>
		<field name="help_topic_id">366</field>
		<field name="help_keyword_id">106</field>
	</row>
	<row>
		<field name="help_topic_id">110</field>
		<field name="help_keyword_id">107</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="help_keyword_id">108</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">108</field>
	</row>
	<row>
		<field name="help_topic_id">462</field>
		<field name="help_keyword_id">108</field>
	</row>
	<row>
		<field name="help_topic_id">24</field>
		<field name="help_keyword_id">109</field>
	</row>
	<row>
		<field name="help_topic_id">89</field>
		<field name="help_keyword_id">109</field>
	</row>
	<row>
		<field name="help_topic_id">114</field>
		<field name="help_keyword_id">110</field>
	</row>
	<row>
		<field name="help_topic_id">175</field>
		<field name="help_keyword_id">110</field>
	</row>
	<row>
		<field name="help_topic_id">3</field>
		<field name="help_keyword_id">111</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">111</field>
	</row>
	<row>
		<field name="help_topic_id">156</field>
		<field name="help_keyword_id">111</field>
	</row>
	<row>
		<field name="help_topic_id">201</field>
		<field name="help_keyword_id">111</field>
	</row>
	<row>
		<field name="help_topic_id">213</field>
		<field name="help_keyword_id">111</field>
	</row>
	<row>
		<field name="help_topic_id">245</field>
		<field name="help_keyword_id">111</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">111</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">111</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">112</field>
	</row>
	<row>
		<field name="help_topic_id">116</field>
		<field name="help_keyword_id">113</field>
	</row>
	<row>
		<field name="help_topic_id">328</field>
		<field name="help_keyword_id">113</field>
	</row>
	<row>
		<field name="help_topic_id">468</field>
		<field name="help_keyword_id">113</field>
	</row>
	<row>
		<field name="help_topic_id">472</field>
		<field name="help_keyword_id">113</field>
	</row>
	<row>
		<field name="help_topic_id">116</field>
		<field name="help_keyword_id">114</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">114</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">115</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">115</field>
	</row>
	<row>
		<field name="help_topic_id">38</field>
		<field name="help_keyword_id">116</field>
	</row>
	<row>
		<field name="help_topic_id">120</field>
		<field name="help_keyword_id">116</field>
	</row>
	<row>
		<field name="help_topic_id">152</field>
		<field name="help_keyword_id">116</field>
	</row>
	<row>
		<field name="help_topic_id">266</field>
		<field name="help_keyword_id">116</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">117</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">117</field>
	</row>
	<row>
		<field name="help_topic_id">420</field>
		<field name="help_keyword_id">118</field>
	</row>
	<row>
		<field name="help_topic_id">125</field>
		<field name="help_keyword_id">119</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">120</field>
	</row>
	<row>
		<field name="help_topic_id">126</field>
		<field name="help_keyword_id">120</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">120</field>
	</row>
	<row>
		<field name="help_topic_id">498</field>
		<field name="help_keyword_id">120</field>
	</row>
	<row>
		<field name="help_topic_id">222</field>
		<field name="help_keyword_id">121</field>
	</row>
	<row>
		<field name="help_topic_id">3</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">84</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">130</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">156</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">183</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">213</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">334</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">479</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">496</field>
		<field name="help_keyword_id">122</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">123</field>
	</row>
	<row>
		<field name="help_topic_id">133</field>
		<field name="help_keyword_id">124</field>
	</row>
	<row>
		<field name="help_topic_id">231</field>
		<field name="help_keyword_id">124</field>
	</row>
	<row>
		<field name="help_topic_id">264</field>
		<field name="help_keyword_id">124</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">124</field>
	</row>
	<row>
		<field name="help_topic_id">59</field>
		<field name="help_keyword_id">125</field>
	</row>
	<row>
		<field name="help_topic_id">138</field>
		<field name="help_keyword_id">125</field>
	</row>
	<row>
		<field name="help_topic_id">216</field>
		<field name="help_keyword_id">125</field>
	</row>
	<row>
		<field name="help_topic_id">224</field>
		<field name="help_keyword_id">125</field>
	</row>
	<row>
		<field name="help_topic_id">330</field>
		<field name="help_keyword_id">125</field>
	</row>
	<row>
		<field name="help_topic_id">337</field>
		<field name="help_keyword_id">125</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">125</field>
	</row>
	<row>
		<field name="help_topic_id">366</field>
		<field name="help_keyword_id">125</field>
	</row>
	<row>
		<field name="help_topic_id">211</field>
		<field name="help_keyword_id">126</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">126</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">126</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">127</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">127</field>
	</row>
	<row>
		<field name="help_topic_id">211</field>
		<field name="help_keyword_id">127</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">127</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">127</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">128</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">129</field>
	</row>
	<row>
		<field name="help_topic_id">425</field>
		<field name="help_keyword_id">130</field>
	</row>
	<row>
		<field name="help_topic_id">53</field>
		<field name="help_keyword_id">131</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">132</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">133</field>
	</row>
	<row>
		<field name="help_topic_id">316</field>
		<field name="help_keyword_id">134</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">135</field>
	</row>
	<row>
		<field name="help_topic_id">457</field>
		<field name="help_keyword_id">136</field>
	</row>
	<row>
		<field name="help_topic_id">257</field>
		<field name="help_keyword_id">137</field>
	</row>
	<row>
		<field name="help_topic_id">287</field>
		<field name="help_keyword_id">138</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">139</field>
	</row>
	<row>
		<field name="help_topic_id">251</field>
		<field name="help_keyword_id">139</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">140</field>
	</row>
	<row>
		<field name="help_topic_id">58</field>
		<field name="help_keyword_id">141</field>
	</row>
	<row>
		<field name="help_topic_id">86</field>
		<field name="help_keyword_id">141</field>
	</row>
	<row>
		<field name="help_topic_id">239</field>
		<field name="help_keyword_id">142</field>
	</row>
	<row>
		<field name="help_topic_id">53</field>
		<field name="help_keyword_id">143</field>
	</row>
	<row>
		<field name="help_topic_id">326</field>
		<field name="help_keyword_id">143</field>
	</row>
	<row>
		<field name="help_topic_id">58</field>
		<field name="help_keyword_id">144</field>
	</row>
	<row>
		<field name="help_topic_id">86</field>
		<field name="help_keyword_id">144</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">145</field>
	</row>
	<row>
		<field name="help_topic_id">315</field>
		<field name="help_keyword_id">146</field>
	</row>
	<row>
		<field name="help_topic_id">49</field>
		<field name="help_keyword_id">147</field>
	</row>
	<row>
		<field name="help_topic_id">123</field>
		<field name="help_keyword_id">147</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">147</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">147</field>
	</row>
	<row>
		<field name="help_topic_id">425</field>
		<field name="help_keyword_id">147</field>
	</row>
	<row>
		<field name="help_topic_id">457</field>
		<field name="help_keyword_id">147</field>
	</row>
	<row>
		<field name="help_topic_id">36</field>
		<field name="help_keyword_id">148</field>
	</row>
	<row>
		<field name="help_topic_id">107</field>
		<field name="help_keyword_id">148</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">148</field>
	</row>
	<row>
		<field name="help_topic_id">449</field>
		<field name="help_keyword_id">148</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">149</field>
	</row>
	<row>
		<field name="help_topic_id">28</field>
		<field name="help_keyword_id">150</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">151</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">152</field>
	</row>
	<row>
		<field name="help_topic_id">210</field>
		<field name="help_keyword_id">153</field>
	</row>
	<row>
		<field name="help_topic_id">181</field>
		<field name="help_keyword_id">154</field>
	</row>
	<row>
		<field name="help_topic_id">315</field>
		<field name="help_keyword_id">154</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">154</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">154</field>
	</row>
	<row>
		<field name="help_topic_id">421</field>
		<field name="help_keyword_id">154</field>
	</row>
	<row>
		<field name="help_topic_id">331</field>
		<field name="help_keyword_id">155</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">155</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">156</field>
	</row>
	<row>
		<field name="help_topic_id">98</field>
		<field name="help_keyword_id">157</field>
	</row>
	<row>
		<field name="help_topic_id">181</field>
		<field name="help_keyword_id">158</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">159</field>
	</row>
	<row>
		<field name="help_topic_id">34</field>
		<field name="help_keyword_id">160</field>
	</row>
	<row>
		<field name="help_topic_id">69</field>
		<field name="help_keyword_id">160</field>
	</row>
	<row>
		<field name="help_topic_id">98</field>
		<field name="help_keyword_id">160</field>
	</row>
	<row>
		<field name="help_topic_id">212</field>
		<field name="help_keyword_id">160</field>
	</row>
	<row>
		<field name="help_topic_id">235</field>
		<field name="help_keyword_id">160</field>
	</row>
	<row>
		<field name="help_topic_id">304</field>
		<field name="help_keyword_id">160</field>
	</row>
	<row>
		<field name="help_topic_id">337</field>
		<field name="help_keyword_id">160</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">160</field>
	</row>
	<row>
		<field name="help_topic_id">394</field>
		<field name="help_keyword_id">160</field>
	</row>
	<row>
		<field name="help_topic_id">406</field>
		<field name="help_keyword_id">160</field>
	</row>
	<row>
		<field name="help_topic_id">417</field>
		<field name="help_keyword_id">160</field>
	</row>
	<row>
		<field name="help_topic_id">98</field>
		<field name="help_keyword_id">161</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">162</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">162</field>
	</row>
	<row>
		<field name="help_topic_id">254</field>
		<field name="help_keyword_id">163</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">164</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">165</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">165</field>
	</row>
	<row>
		<field name="help_topic_id">61</field>
		<field name="help_keyword_id">166</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">166</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">166</field>
	</row>
	<row>
		<field name="help_topic_id">480</field>
		<field name="help_keyword_id">166</field>
	</row>
	<row>
		<field name="help_topic_id">177</field>
		<field name="help_keyword_id">167</field>
	</row>
	<row>
		<field name="help_topic_id">169</field>
		<field name="help_keyword_id">168</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">169</field>
	</row>
	<row>
		<field name="help_topic_id">416</field>
		<field name="help_keyword_id">169</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">169</field>
	</row>
	<row>
		<field name="help_topic_id">480</field>
		<field name="help_keyword_id">169</field>
	</row>
	<row>
		<field name="help_topic_id">30</field>
		<field name="help_keyword_id">170</field>
	</row>
	<row>
		<field name="help_topic_id">160</field>
		<field name="help_keyword_id">170</field>
	</row>
	<row>
		<field name="help_topic_id">456</field>
		<field name="help_keyword_id">170</field>
	</row>
	<row>
		<field name="help_topic_id">449</field>
		<field name="help_keyword_id">171</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="help_keyword_id">172</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">172</field>
	</row>
	<row>
		<field name="help_topic_id">191</field>
		<field name="help_keyword_id">173</field>
	</row>
	<row>
		<field name="help_topic_id">49</field>
		<field name="help_keyword_id">174</field>
	</row>
	<row>
		<field name="help_topic_id">84</field>
		<field name="help_keyword_id">174</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">174</field>
	</row>
	<row>
		<field name="help_topic_id">362</field>
		<field name="help_keyword_id">174</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">174</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">175</field>
	</row>
	<row>
		<field name="help_topic_id">49</field>
		<field name="help_keyword_id">175</field>
	</row>
	<row>
		<field name="help_topic_id">87</field>
		<field name="help_keyword_id">175</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">176</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">176</field>
	</row>
	<row>
		<field name="help_topic_id">254</field>
		<field name="help_keyword_id">177</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">178</field>
	</row>
	<row>
		<field name="help_topic_id">251</field>
		<field name="help_keyword_id">178</field>
	</row>
	<row>
		<field name="help_topic_id">24</field>
		<field name="help_keyword_id">179</field>
	</row>
	<row>
		<field name="help_topic_id">129</field>
		<field name="help_keyword_id">179</field>
	</row>
	<row>
		<field name="help_topic_id">169</field>
		<field name="help_keyword_id">179</field>
	</row>
	<row>
		<field name="help_topic_id">210</field>
		<field name="help_keyword_id">179</field>
	</row>
	<row>
		<field name="help_topic_id">231</field>
		<field name="help_keyword_id">179</field>
	</row>
	<row>
		<field name="help_topic_id">316</field>
		<field name="help_keyword_id">179</field>
	</row>
	<row>
		<field name="help_topic_id">502</field>
		<field name="help_keyword_id">179</field>
	</row>
	<row>
		<field name="help_topic_id">98</field>
		<field name="help_keyword_id">180</field>
	</row>
	<row>
		<field name="help_topic_id">157</field>
		<field name="help_keyword_id">180</field>
	</row>
	<row>
		<field name="help_topic_id">231</field>
		<field name="help_keyword_id">180</field>
	</row>
	<row>
		<field name="help_topic_id">412</field>
		<field name="help_keyword_id">181</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">182</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">183</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">183</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="help_keyword_id">183</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">183</field>
	</row>
	<row>
		<field name="help_topic_id">102</field>
		<field name="help_keyword_id">184</field>
	</row>
	<row>
		<field name="help_topic_id">152</field>
		<field name="help_keyword_id">184</field>
	</row>
	<row>
		<field name="help_topic_id">306</field>
		<field name="help_keyword_id">184</field>
	</row>
	<row>
		<field name="help_topic_id">146</field>
		<field name="help_keyword_id">185</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">185</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">186</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">186</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="help_keyword_id">187</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">187</field>
	</row>
	<row>
		<field name="help_topic_id">161</field>
		<field name="help_keyword_id">188</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">188</field>
	</row>
	<row>
		<field name="help_topic_id">457</field>
		<field name="help_keyword_id">189</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="help_keyword_id">190</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">190</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">191</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">191</field>
	</row>
	<row>
		<field name="help_topic_id">181</field>
		<field name="help_keyword_id">192</field>
	</row>
	<row>
		<field name="help_topic_id">201</field>
		<field name="help_keyword_id">192</field>
	</row>
	<row>
		<field name="help_topic_id">315</field>
		<field name="help_keyword_id">192</field>
	</row>
	<row>
		<field name="help_topic_id">421</field>
		<field name="help_keyword_id">192</field>
	</row>
	<row>
		<field name="help_topic_id">306</field>
		<field name="help_keyword_id">193</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">193</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">194</field>
	</row>
	<row>
		<field name="help_topic_id">231</field>
		<field name="help_keyword_id">195</field>
	</row>
	<row>
		<field name="help_topic_id">378</field>
		<field name="help_keyword_id">195</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">196</field>
	</row>
	<row>
		<field name="help_topic_id">156</field>
		<field name="help_keyword_id">197</field>
	</row>
	<row>
		<field name="help_topic_id">213</field>
		<field name="help_keyword_id">197</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">197</field>
	</row>
	<row>
		<field name="help_topic_id">396</field>
		<field name="help_keyword_id">198</field>
	</row>
	<row>
		<field name="help_topic_id">467</field>
		<field name="help_keyword_id">199</field>
	</row>
	<row>
		<field name="help_topic_id">130</field>
		<field name="help_keyword_id">200</field>
	</row>
	<row>
		<field name="help_topic_id">138</field>
		<field name="help_keyword_id">200</field>
	</row>
	<row>
		<field name="help_topic_id">183</field>
		<field name="help_keyword_id">200</field>
	</row>
	<row>
		<field name="help_topic_id">350</field>
		<field name="help_keyword_id">200</field>
	</row>
	<row>
		<field name="help_topic_id">449</field>
		<field name="help_keyword_id">200</field>
	</row>
	<row>
		<field name="help_topic_id">454</field>
		<field name="help_keyword_id">201</field>
	</row>
	<row>
		<field name="help_topic_id">58</field>
		<field name="help_keyword_id">202</field>
	</row>
	<row>
		<field name="help_topic_id">86</field>
		<field name="help_keyword_id">202</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">203</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">204</field>
	</row>
	<row>
		<field name="help_topic_id">36</field>
		<field name="help_keyword_id">204</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">204</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">205</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">206</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="help_keyword_id">207</field>
	</row>
	<row>
		<field name="help_topic_id">251</field>
		<field name="help_keyword_id">208</field>
	</row>
	<row>
		<field name="help_topic_id">193</field>
		<field name="help_keyword_id">209</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">209</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">210</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">211</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">211</field>
	</row>
	<row>
		<field name="help_topic_id">222</field>
		<field name="help_keyword_id">212</field>
	</row>
	<row>
		<field name="help_topic_id">257</field>
		<field name="help_keyword_id">213</field>
	</row>
	<row>
		<field name="help_topic_id">89</field>
		<field name="help_keyword_id">214</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">214</field>
	</row>
	<row>
		<field name="help_topic_id">211</field>
		<field name="help_keyword_id">214</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">214</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">214</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">215</field>
	</row>
	<row>
		<field name="help_topic_id">262</field>
		<field name="help_keyword_id">216</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">217</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">217</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">218</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">218</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">218</field>
	</row>
	<row>
		<field name="help_topic_id">293</field>
		<field name="help_keyword_id">219</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">220</field>
	</row>
	<row>
		<field name="help_topic_id">3</field>
		<field name="help_keyword_id">221</field>
	</row>
	<row>
		<field name="help_topic_id">36</field>
		<field name="help_keyword_id">221</field>
	</row>
	<row>
		<field name="help_topic_id">49</field>
		<field name="help_keyword_id">221</field>
	</row>
	<row>
		<field name="help_topic_id">84</field>
		<field name="help_keyword_id">221</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">221</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="help_keyword_id">221</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">221</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">222</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">223</field>
	</row>
	<row>
		<field name="help_topic_id">156</field>
		<field name="help_keyword_id">224</field>
	</row>
	<row>
		<field name="help_topic_id">187</field>
		<field name="help_keyword_id">224</field>
	</row>
	<row>
		<field name="help_topic_id">213</field>
		<field name="help_keyword_id">224</field>
	</row>
	<row>
		<field name="help_topic_id">311</field>
		<field name="help_keyword_id">224</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">224</field>
	</row>
	<row>
		<field name="help_topic_id">98</field>
		<field name="help_keyword_id">225</field>
	</row>
	<row>
		<field name="help_topic_id">279</field>
		<field name="help_keyword_id">226</field>
	</row>
	<row>
		<field name="help_topic_id">281</field>
		<field name="help_keyword_id">227</field>
	</row>
	<row>
		<field name="help_topic_id">466</field>
		<field name="help_keyword_id">228</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">229</field>
	</row>
	<row>
		<field name="help_topic_id">61</field>
		<field name="help_keyword_id">229</field>
	</row>
	<row>
		<field name="help_topic_id">88</field>
		<field name="help_keyword_id">229</field>
	</row>
	<row>
		<field name="help_topic_id">102</field>
		<field name="help_keyword_id">229</field>
	</row>
	<row>
		<field name="help_topic_id">211</field>
		<field name="help_keyword_id">229</field>
	</row>
	<row>
		<field name="help_topic_id">306</field>
		<field name="help_keyword_id">229</field>
	</row>
	<row>
		<field name="help_topic_id">310</field>
		<field name="help_keyword_id">229</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">229</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">229</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">229</field>
	</row>
	<row>
		<field name="help_topic_id">426</field>
		<field name="help_keyword_id">230</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">231</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">231</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">232</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">233</field>
	</row>
	<row>
		<field name="help_topic_id">257</field>
		<field name="help_keyword_id">234</field>
	</row>
	<row>
		<field name="help_topic_id">468</field>
		<field name="help_keyword_id">234</field>
	</row>
	<row>
		<field name="help_topic_id">274</field>
		<field name="help_keyword_id">235</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">236</field>
	</row>
	<row>
		<field name="help_topic_id">7</field>
		<field name="help_keyword_id">237</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">237</field>
	</row>
	<row>
		<field name="help_topic_id">259</field>
		<field name="help_keyword_id">238</field>
	</row>
	<row>
		<field name="help_topic_id">431</field>
		<field name="help_keyword_id">238</field>
	</row>
	<row>
		<field name="help_topic_id">48</field>
		<field name="help_keyword_id">239</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">239</field>
	</row>
	<row>
		<field name="help_topic_id">227</field>
		<field name="help_keyword_id">239</field>
	</row>
	<row>
		<field name="help_topic_id">232</field>
		<field name="help_keyword_id">239</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">239</field>
	</row>
	<row>
		<field name="help_topic_id">428</field>
		<field name="help_keyword_id">240</field>
	</row>
	<row>
		<field name="help_topic_id">88</field>
		<field name="help_keyword_id">241</field>
	</row>
	<row>
		<field name="help_topic_id">211</field>
		<field name="help_keyword_id">241</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">241</field>
	</row>
	<row>
		<field name="help_topic_id">315</field>
		<field name="help_keyword_id">242</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">243</field>
	</row>
	<row>
		<field name="help_topic_id">24</field>
		<field name="help_keyword_id">244</field>
	</row>
	<row>
		<field name="help_topic_id">129</field>
		<field name="help_keyword_id">244</field>
	</row>
	<row>
		<field name="help_topic_id">169</field>
		<field name="help_keyword_id">244</field>
	</row>
	<row>
		<field name="help_topic_id">210</field>
		<field name="help_keyword_id">244</field>
	</row>
	<row>
		<field name="help_topic_id">316</field>
		<field name="help_keyword_id">244</field>
	</row>
	<row>
		<field name="help_topic_id">502</field>
		<field name="help_keyword_id">244</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">245</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">246</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">247</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">248</field>
	</row>
	<row>
		<field name="help_topic_id">386</field>
		<field name="help_keyword_id">249</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="help_keyword_id">250</field>
	</row>
	<row>
		<field name="help_topic_id">156</field>
		<field name="help_keyword_id">250</field>
	</row>
	<row>
		<field name="help_topic_id">213</field>
		<field name="help_keyword_id">250</field>
	</row>
	<row>
		<field name="help_topic_id">259</field>
		<field name="help_keyword_id">250</field>
	</row>
	<row>
		<field name="help_topic_id">334</field>
		<field name="help_keyword_id">250</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">250</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">250</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">250</field>
	</row>
	<row>
		<field name="help_topic_id">431</field>
		<field name="help_keyword_id">250</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">250</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">251</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">252</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">252</field>
	</row>
	<row>
		<field name="help_topic_id">61</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">111</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">116</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">216</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">278</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">280</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">323</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">402</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">418</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">468</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">472</field>
		<field name="help_keyword_id">253</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">254</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">254</field>
	</row>
	<row>
		<field name="help_topic_id">366</field>
		<field name="help_keyword_id">254</field>
	</row>
	<row>
		<field name="help_topic_id">416</field>
		<field name="help_keyword_id">254</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">254</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">254</field>
	</row>
	<row>
		<field name="help_topic_id">480</field>
		<field name="help_keyword_id">254</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">255</field>
	</row>
	<row>
		<field name="help_topic_id">30</field>
		<field name="help_keyword_id">256</field>
	</row>
	<row>
		<field name="help_topic_id">278</field>
		<field name="help_keyword_id">256</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">256</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="help_keyword_id">256</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">257</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">258</field>
	</row>
	<row>
		<field name="help_topic_id">307</field>
		<field name="help_keyword_id">259</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">260</field>
	</row>
	<row>
		<field name="help_topic_id">312</field>
		<field name="help_keyword_id">261</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">262</field>
	</row>
	<row>
		<field name="help_topic_id">89</field>
		<field name="help_keyword_id">263</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">264</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">265</field>
	</row>
	<row>
		<field name="help_topic_id">257</field>
		<field name="help_keyword_id">266</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">266</field>
	</row>
	<row>
		<field name="help_topic_id">362</field>
		<field name="help_keyword_id">266</field>
	</row>
	<row>
		<field name="help_topic_id">231</field>
		<field name="help_keyword_id">267</field>
	</row>
	<row>
		<field name="help_topic_id">317</field>
		<field name="help_keyword_id">267</field>
	</row>
	<row>
		<field name="help_topic_id">376</field>
		<field name="help_keyword_id">267</field>
	</row>
	<row>
		<field name="help_topic_id">210</field>
		<field name="help_keyword_id">268</field>
	</row>
	<row>
		<field name="help_topic_id">89</field>
		<field name="help_keyword_id">269</field>
	</row>
	<row>
		<field name="help_topic_id">69</field>
		<field name="help_keyword_id">270</field>
	</row>
	<row>
		<field name="help_topic_id">490</field>
		<field name="help_keyword_id">270</field>
	</row>
	<row>
		<field name="help_topic_id">421</field>
		<field name="help_keyword_id">271</field>
	</row>
	<row>
		<field name="help_topic_id">248</field>
		<field name="help_keyword_id">272</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="help_keyword_id">273</field>
	</row>
	<row>
		<field name="help_topic_id">48</field>
		<field name="help_keyword_id">274</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">274</field>
	</row>
	<row>
		<field name="help_topic_id">227</field>
		<field name="help_keyword_id">274</field>
	</row>
	<row>
		<field name="help_topic_id">232</field>
		<field name="help_keyword_id">274</field>
	</row>
	<row>
		<field name="help_topic_id">152</field>
		<field name="help_keyword_id">275</field>
	</row>
	<row>
		<field name="help_topic_id">328</field>
		<field name="help_keyword_id">275</field>
	</row>
	<row>
		<field name="help_topic_id">17</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">22</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">48</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">61</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">78</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">98</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">156</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">211</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">212</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">280</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">304</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">311</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">359</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">394</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">456</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">276</field>
	</row>
	<row>
		<field name="help_topic_id">257</field>
		<field name="help_keyword_id">277</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">278</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">279</field>
	</row>
	<row>
		<field name="help_topic_id">236</field>
		<field name="help_keyword_id">280</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">281</field>
	</row>
	<row>
		<field name="help_topic_id">453</field>
		<field name="help_keyword_id">281</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">282</field>
	</row>
	<row>
		<field name="help_topic_id">39</field>
		<field name="help_keyword_id">283</field>
	</row>
	<row>
		<field name="help_topic_id">271</field>
		<field name="help_keyword_id">283</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">283</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">284</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">285</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">286</field>
	</row>
	<row>
		<field name="help_topic_id">379</field>
		<field name="help_keyword_id">287</field>
	</row>
	<row>
		<field name="help_topic_id">148</field>
		<field name="help_keyword_id">288</field>
	</row>
	<row>
		<field name="help_topic_id">108</field>
		<field name="help_keyword_id">289</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">290</field>
	</row>
	<row>
		<field name="help_topic_id">468</field>
		<field name="help_keyword_id">290</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">291</field>
	</row>
	<row>
		<field name="help_topic_id">418</field>
		<field name="help_keyword_id">292</field>
	</row>
	<row>
		<field name="help_topic_id">3</field>
		<field name="help_keyword_id">293</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">293</field>
	</row>
	<row>
		<field name="help_topic_id">340</field>
		<field name="help_keyword_id">294</field>
	</row>
	<row>
		<field name="help_topic_id">345</field>
		<field name="help_keyword_id">295</field>
	</row>
	<row>
		<field name="help_topic_id">259</field>
		<field name="help_keyword_id">296</field>
	</row>
	<row>
		<field name="help_topic_id">271</field>
		<field name="help_keyword_id">297</field>
	</row>
	<row>
		<field name="help_topic_id">323</field>
		<field name="help_keyword_id">298</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">298</field>
	</row>
	<row>
		<field name="help_topic_id">7</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">10</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">17</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">21</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">25</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">33</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">39</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">59</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">69</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">70</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">83</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">123</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">135</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">138</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">146</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">161</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">171</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">193</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">194</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">216</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">224</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">280</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">284</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">296</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">310</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">311</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">330</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">331</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">334</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">337</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">350</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">359</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">366</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">394</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">425</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">453</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">489</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">490</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">494</field>
		<field name="help_keyword_id">299</field>
	</row>
	<row>
		<field name="help_topic_id">38</field>
		<field name="help_keyword_id">300</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">300</field>
	</row>
	<row>
		<field name="help_topic_id">251</field>
		<field name="help_keyword_id">300</field>
	</row>
	<row>
		<field name="help_topic_id">307</field>
		<field name="help_keyword_id">300</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">300</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">301</field>
	</row>
	<row>
		<field name="help_topic_id">328</field>
		<field name="help_keyword_id">302</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">303</field>
	</row>
	<row>
		<field name="help_topic_id">123</field>
		<field name="help_keyword_id">304</field>
	</row>
	<row>
		<field name="help_topic_id">351</field>
		<field name="help_keyword_id">304</field>
	</row>
	<row>
		<field name="help_topic_id">28</field>
		<field name="help_keyword_id">305</field>
	</row>
	<row>
		<field name="help_topic_id">58</field>
		<field name="help_keyword_id">305</field>
	</row>
	<row>
		<field name="help_topic_id">86</field>
		<field name="help_keyword_id">305</field>
	</row>
	<row>
		<field name="help_topic_id">234</field>
		<field name="help_keyword_id">305</field>
	</row>
	<row>
		<field name="help_topic_id">329</field>
		<field name="help_keyword_id">305</field>
	</row>
	<row>
		<field name="help_topic_id">345</field>
		<field name="help_keyword_id">305</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">305</field>
	</row>
	<row>
		<field name="help_topic_id">498</field>
		<field name="help_keyword_id">305</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">306</field>
	</row>
	<row>
		<field name="help_topic_id">148</field>
		<field name="help_keyword_id">307</field>
	</row>
	<row>
		<field name="help_topic_id">318</field>
		<field name="help_keyword_id">307</field>
	</row>
	<row>
		<field name="help_topic_id">316</field>
		<field name="help_keyword_id">308</field>
	</row>
	<row>
		<field name="help_topic_id">107</field>
		<field name="help_keyword_id">309</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">310</field>
	</row>
	<row>
		<field name="help_topic_id">3</field>
		<field name="help_keyword_id">311</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">311</field>
	</row>
	<row>
		<field name="help_topic_id">257</field>
		<field name="help_keyword_id">311</field>
	</row>
	<row>
		<field name="help_topic_id">303</field>
		<field name="help_keyword_id">311</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">311</field>
	</row>
	<row>
		<field name="help_topic_id">161</field>
		<field name="help_keyword_id">312</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">312</field>
	</row>
	<row>
		<field name="help_topic_id">142</field>
		<field name="help_keyword_id">313</field>
	</row>
	<row>
		<field name="help_topic_id">78</field>
		<field name="help_keyword_id">314</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="help_keyword_id">314</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">314</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">315</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">316</field>
	</row>
	<row>
		<field name="help_topic_id">183</field>
		<field name="help_keyword_id">317</field>
	</row>
	<row>
		<field name="help_topic_id">457</field>
		<field name="help_keyword_id">318</field>
	</row>
	<row>
		<field name="help_topic_id">24</field>
		<field name="help_keyword_id">319</field>
	</row>
	<row>
		<field name="help_topic_id">112</field>
		<field name="help_keyword_id">319</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">320</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">321</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="help_keyword_id">322</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">322</field>
	</row>
	<row>
		<field name="help_topic_id">119</field>
		<field name="help_keyword_id">323</field>
	</row>
	<row>
		<field name="help_topic_id">391</field>
		<field name="help_keyword_id">323</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">324</field>
	</row>
	<row>
		<field name="help_topic_id">25</field>
		<field name="help_keyword_id">325</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">325</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">326</field>
	</row>
	<row>
		<field name="help_topic_id">374</field>
		<field name="help_keyword_id">327</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">327</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">328</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="help_keyword_id">329</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">329</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">330</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">331</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">332</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">333</field>
	</row>
	<row>
		<field name="help_topic_id">379</field>
		<field name="help_keyword_id">333</field>
	</row>
	<row>
		<field name="help_topic_id">293</field>
		<field name="help_keyword_id">334</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">334</field>
	</row>
	<row>
		<field name="help_topic_id">420</field>
		<field name="help_keyword_id">334</field>
	</row>
	<row>
		<field name="help_topic_id">384</field>
		<field name="help_keyword_id">335</field>
	</row>
	<row>
		<field name="help_topic_id">89</field>
		<field name="help_keyword_id">336</field>
	</row>
	<row>
		<field name="help_topic_id">123</field>
		<field name="help_keyword_id">336</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">336</field>
	</row>
	<row>
		<field name="help_topic_id">425</field>
		<field name="help_keyword_id">336</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">337</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">338</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">339</field>
	</row>
	<row>
		<field name="help_topic_id">87</field>
		<field name="help_keyword_id">340</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">340</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">341</field>
	</row>
	<row>
		<field name="help_topic_id">398</field>
		<field name="help_keyword_id">342</field>
	</row>
	<row>
		<field name="help_topic_id">89</field>
		<field name="help_keyword_id">343</field>
	</row>
	<row>
		<field name="help_topic_id">152</field>
		<field name="help_keyword_id">343</field>
	</row>
	<row>
		<field name="help_topic_id">177</field>
		<field name="help_keyword_id">343</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">344</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="help_keyword_id">345</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">345</field>
	</row>
	<row>
		<field name="help_topic_id">53</field>
		<field name="help_keyword_id">346</field>
	</row>
	<row>
		<field name="help_topic_id">326</field>
		<field name="help_keyword_id">346</field>
	</row>
	<row>
		<field name="help_topic_id">9</field>
		<field name="help_keyword_id">347</field>
	</row>
	<row>
		<field name="help_topic_id">374</field>
		<field name="help_keyword_id">347</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">347</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">348</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">349</field>
	</row>
	<row>
		<field name="help_topic_id">124</field>
		<field name="help_keyword_id">350</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="help_keyword_id">351</field>
	</row>
	<row>
		<field name="help_topic_id">449</field>
		<field name="help_keyword_id">351</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">352</field>
	</row>
	<row>
		<field name="help_topic_id">408</field>
		<field name="help_keyword_id">353</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">354</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">355</field>
	</row>
	<row>
		<field name="help_topic_id">145</field>
		<field name="help_keyword_id">356</field>
	</row>
	<row>
		<field name="help_topic_id">24</field>
		<field name="help_keyword_id">357</field>
	</row>
	<row>
		<field name="help_topic_id">111</field>
		<field name="help_keyword_id">358</field>
	</row>
	<row>
		<field name="help_topic_id">223</field>
		<field name="help_keyword_id">358</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">358</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">358</field>
	</row>
	<row>
		<field name="help_topic_id">211</field>
		<field name="help_keyword_id">359</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">359</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">359</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">360</field>
	</row>
	<row>
		<field name="help_topic_id">61</field>
		<field name="help_keyword_id">361</field>
	</row>
	<row>
		<field name="help_topic_id">160</field>
		<field name="help_keyword_id">361</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">361</field>
	</row>
	<row>
		<field name="help_topic_id">213</field>
		<field name="help_keyword_id">361</field>
	</row>
	<row>
		<field name="help_topic_id">227</field>
		<field name="help_keyword_id">361</field>
	</row>
	<row>
		<field name="help_topic_id">235</field>
		<field name="help_keyword_id">361</field>
	</row>
	<row>
		<field name="help_topic_id">374</field>
		<field name="help_keyword_id">361</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">361</field>
	</row>
	<row>
		<field name="help_topic_id">441</field>
		<field name="help_keyword_id">361</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">361</field>
	</row>
	<row>
		<field name="help_topic_id">480</field>
		<field name="help_keyword_id">361</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">362</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">362</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">363</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">364</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">364</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="help_keyword_id">365</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">366</field>
	</row>
	<row>
		<field name="help_topic_id">350</field>
		<field name="help_keyword_id">366</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">367</field>
	</row>
	<row>
		<field name="help_topic_id">428</field>
		<field name="help_keyword_id">368</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">369</field>
	</row>
	<row>
		<field name="help_topic_id">78</field>
		<field name="help_keyword_id">370</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">370</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">370</field>
	</row>
	<row>
		<field name="help_topic_id">479</field>
		<field name="help_keyword_id">370</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">371</field>
	</row>
	<row>
		<field name="help_topic_id">231</field>
		<field name="help_keyword_id">372</field>
	</row>
	<row>
		<field name="help_topic_id">467</field>
		<field name="help_keyword_id">372</field>
	</row>
	<row>
		<field name="help_topic_id">107</field>
		<field name="help_keyword_id">373</field>
	</row>
	<row>
		<field name="help_topic_id">70</field>
		<field name="help_keyword_id">374</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">374</field>
	</row>
	<row>
		<field name="help_topic_id">496</field>
		<field name="help_keyword_id">375</field>
	</row>
	<row>
		<field name="help_topic_id">278</field>
		<field name="help_keyword_id">376</field>
	</row>
	<row>
		<field name="help_topic_id">449</field>
		<field name="help_keyword_id">377</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">378</field>
	</row>
	<row>
		<field name="help_topic_id">213</field>
		<field name="help_keyword_id">379</field>
	</row>
	<row>
		<field name="help_topic_id">418</field>
		<field name="help_keyword_id">379</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">380</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="help_keyword_id">381</field>
	</row>
	<row>
		<field name="help_topic_id">329</field>
		<field name="help_keyword_id">381</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">381</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">382</field>
	</row>
	<row>
		<field name="help_topic_id">489</field>
		<field name="help_keyword_id">383</field>
	</row>
	<row>
		<field name="help_topic_id">418</field>
		<field name="help_keyword_id">384</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">385</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">385</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">386</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="help_keyword_id">387</field>
	</row>
	<row>
		<field name="help_topic_id">213</field>
		<field name="help_keyword_id">388</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">389</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">389</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="help_keyword_id">389</field>
	</row>
	<row>
		<field name="help_topic_id">445</field>
		<field name="help_keyword_id">390</field>
	</row>
	<row>
		<field name="help_topic_id">284</field>
		<field name="help_keyword_id">391</field>
	</row>
	<row>
		<field name="help_topic_id">449</field>
		<field name="help_keyword_id">392</field>
	</row>
	<row>
		<field name="help_topic_id">450</field>
		<field name="help_keyword_id">393</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">394</field>
	</row>
	<row>
		<field name="help_topic_id">129</field>
		<field name="help_keyword_id">395</field>
	</row>
	<row>
		<field name="help_topic_id">10</field>
		<field name="help_keyword_id">396</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">396</field>
	</row>
	<row>
		<field name="help_topic_id">30</field>
		<field name="help_keyword_id">397</field>
	</row>
	<row>
		<field name="help_topic_id">278</field>
		<field name="help_keyword_id">397</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="help_keyword_id">397</field>
	</row>
	<row>
		<field name="help_topic_id">449</field>
		<field name="help_keyword_id">398</field>
	</row>
	<row>
		<field name="help_topic_id">57</field>
		<field name="help_keyword_id">399</field>
	</row>
	<row>
		<field name="help_topic_id">85</field>
		<field name="help_keyword_id">400</field>
	</row>
	<row>
		<field name="help_topic_id">207</field>
		<field name="help_keyword_id">400</field>
	</row>
	<row>
		<field name="help_topic_id">369</field>
		<field name="help_keyword_id">400</field>
	</row>
	<row>
		<field name="help_topic_id">458</field>
		<field name="help_keyword_id">400</field>
	</row>
	<row>
		<field name="help_topic_id">85</field>
		<field name="help_keyword_id">401</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">401</field>
	</row>
	<row>
		<field name="help_topic_id">156</field>
		<field name="help_keyword_id">401</field>
	</row>
	<row>
		<field name="help_topic_id">207</field>
		<field name="help_keyword_id">401</field>
	</row>
	<row>
		<field name="help_topic_id">314</field>
		<field name="help_keyword_id">401</field>
	</row>
	<row>
		<field name="help_topic_id">190</field>
		<field name="help_keyword_id">402</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">403</field>
	</row>
	<row>
		<field name="help_topic_id">480</field>
		<field name="help_keyword_id">403</field>
	</row>
	<row>
		<field name="help_topic_id">328</field>
		<field name="help_keyword_id">404</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">405</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">406</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="help_keyword_id">407</field>
	</row>
	<row>
		<field name="help_topic_id">326</field>
		<field name="help_keyword_id">407</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">407</field>
	</row>
	<row>
		<field name="help_topic_id">33</field>
		<field name="help_keyword_id">408</field>
	</row>
	<row>
		<field name="help_topic_id">462</field>
		<field name="help_keyword_id">409</field>
	</row>
	<row>
		<field name="help_topic_id">9</field>
		<field name="help_keyword_id">410</field>
	</row>
	<row>
		<field name="help_topic_id">28</field>
		<field name="help_keyword_id">410</field>
	</row>
	<row>
		<field name="help_topic_id">30</field>
		<field name="help_keyword_id">410</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">410</field>
	</row>
	<row>
		<field name="help_topic_id">156</field>
		<field name="help_keyword_id">410</field>
	</row>
	<row>
		<field name="help_topic_id">187</field>
		<field name="help_keyword_id">410</field>
	</row>
	<row>
		<field name="help_topic_id">278</field>
		<field name="help_keyword_id">410</field>
	</row>
	<row>
		<field name="help_topic_id">302</field>
		<field name="help_keyword_id">410</field>
	</row>
	<row>
		<field name="help_topic_id">481</field>
		<field name="help_keyword_id">410</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="help_keyword_id">411</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">412</field>
	</row>
	<row>
		<field name="help_topic_id">271</field>
		<field name="help_keyword_id">413</field>
	</row>
	<row>
		<field name="help_topic_id">107</field>
		<field name="help_keyword_id">414</field>
	</row>
	<row>
		<field name="help_topic_id">78</field>
		<field name="help_keyword_id">415</field>
	</row>
	<row>
		<field name="help_topic_id">223</field>
		<field name="help_keyword_id">415</field>
	</row>
	<row>
		<field name="help_topic_id">332</field>
		<field name="help_keyword_id">415</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">415</field>
	</row>
	<row>
		<field name="help_topic_id">315</field>
		<field name="help_keyword_id">416</field>
	</row>
	<row>
		<field name="help_topic_id">310</field>
		<field name="help_keyword_id">417</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">417</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">417</field>
	</row>
	<row>
		<field name="help_topic_id">49</field>
		<field name="help_keyword_id">418</field>
	</row>
	<row>
		<field name="help_topic_id">84</field>
		<field name="help_keyword_id">418</field>
	</row>
	<row>
		<field name="help_topic_id">107</field>
		<field name="help_keyword_id">418</field>
	</row>
	<row>
		<field name="help_topic_id">123</field>
		<field name="help_keyword_id">418</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">418</field>
	</row>
	<row>
		<field name="help_topic_id">425</field>
		<field name="help_keyword_id">418</field>
	</row>
	<row>
		<field name="help_topic_id">61</field>
		<field name="help_keyword_id">419</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">419</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">419</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">419</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="help_keyword_id">419</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">420</field>
	</row>
	<row>
		<field name="help_topic_id">234</field>
		<field name="help_keyword_id">421</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">422</field>
	</row>
	<row>
		<field name="help_topic_id">3</field>
		<field name="help_keyword_id">423</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">423</field>
	</row>
	<row>
		<field name="help_topic_id">488</field>
		<field name="help_keyword_id">423</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">424</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">424</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">425</field>
	</row>
	<row>
		<field name="help_topic_id">472</field>
		<field name="help_keyword_id">425</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">426</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">426</field>
	</row>
	<row>
		<field name="help_topic_id">245</field>
		<field name="help_keyword_id">427</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">427</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">428</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="help_keyword_id">428</field>
	</row>
	<row>
		<field name="help_topic_id">36</field>
		<field name="help_keyword_id">429</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="help_keyword_id">429</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">430</field>
	</row>
	<row>
		<field name="help_topic_id">480</field>
		<field name="help_keyword_id">430</field>
	</row>
	<row>
		<field name="help_topic_id">130</field>
		<field name="help_keyword_id">431</field>
	</row>
	<row>
		<field name="help_topic_id">138</field>
		<field name="help_keyword_id">431</field>
	</row>
	<row>
		<field name="help_topic_id">350</field>
		<field name="help_keyword_id">431</field>
	</row>
	<row>
		<field name="help_topic_id">449</field>
		<field name="help_keyword_id">431</field>
	</row>
	<row>
		<field name="help_topic_id">156</field>
		<field name="help_keyword_id">432</field>
	</row>
	<row>
		<field name="help_topic_id">187</field>
		<field name="help_keyword_id">432</field>
	</row>
	<row>
		<field name="help_topic_id">213</field>
		<field name="help_keyword_id">432</field>
	</row>
	<row>
		<field name="help_topic_id">311</field>
		<field name="help_keyword_id">432</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">432</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="help_keyword_id">432</field>
	</row>
	<row>
		<field name="help_topic_id">85</field>
		<field name="help_keyword_id">433</field>
	</row>
	<row>
		<field name="help_topic_id">369</field>
		<field name="help_keyword_id">433</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="help_keyword_id">433</field>
	</row>
	<row>
		<field name="help_topic_id">485</field>
		<field name="help_keyword_id">434</field>
	</row>
	<row>
		<field name="help_topic_id">468</field>
		<field name="help_keyword_id">435</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">436</field>
	</row>
	<row>
		<field name="help_topic_id">38</field>
		<field name="help_keyword_id">437</field>
	</row>
	<row>
		<field name="help_topic_id">53</field>
		<field name="help_keyword_id">437</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">437</field>
	</row>
	<row>
		<field name="help_topic_id">146</field>
		<field name="help_keyword_id">437</field>
	</row>
	<row>
		<field name="help_topic_id">224</field>
		<field name="help_keyword_id">437</field>
	</row>
	<row>
		<field name="help_topic_id">326</field>
		<field name="help_keyword_id">437</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">437</field>
	</row>
	<row>
		<field name="help_topic_id">259</field>
		<field name="help_keyword_id">438</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">439</field>
	</row>
	<row>
		<field name="help_topic_id">362</field>
		<field name="help_keyword_id">439</field>
	</row>
	<row>
		<field name="help_topic_id">493</field>
		<field name="help_keyword_id">440</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">441</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="help_keyword_id">442</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="help_keyword_id">442</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">442</field>
	</row>
	<row>
		<field name="help_topic_id">213</field>
		<field name="help_keyword_id">443</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">443</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">444</field>
	</row>
	<row>
		<field name="help_topic_id">498</field>
		<field name="help_keyword_id">445</field>
	</row>
	<row>
		<field name="help_topic_id">36</field>
		<field name="help_keyword_id">446</field>
	</row>
	<row>
		<field name="help_topic_id">116</field>
		<field name="help_keyword_id">446</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="help_keyword_id">446</field>
	</row>
	<row>
		<field name="help_topic_id">328</field>
		<field name="help_keyword_id">446</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="help_keyword_id">446</field>
	</row>
	<row>
		<field name="help_topic_id">468</field>
		<field name="help_keyword_id">446</field>
	</row>
	<row>
		<field name="help_topic_id">472</field>
		<field name="help_keyword_id">446</field>
	</row>
	<row>
		<field name="help_topic_id">0</field>
		<field name="help_keyword_id">447</field>
	</row>
	<row>
		<field name="help_topic_id">97</field>
		<field name="help_keyword_id">447</field>
	</row>
	<row>
		<field name="help_topic_id">290</field>
		<field name="help_keyword_id">447</field>
	</row>
	<row>
		<field name="help_topic_id">307</field>
		<field name="help_keyword_id">447</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">447</field>
	</row>
	<row>
		<field name="help_topic_id">362</field>
		<field name="help_keyword_id">447</field>
	</row>
	<row>
		<field name="help_topic_id">385</field>
		<field name="help_keyword_id">447</field>
	</row>
	<row>
		<field name="help_topic_id">434</field>
		<field name="help_keyword_id">447</field>
	</row>
	<row>
		<field name="help_topic_id">499</field>
		<field name="help_keyword_id">448</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">449</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">450</field>
	</row>
	<row>
		<field name="help_topic_id">36</field>
		<field name="help_keyword_id">451</field>
	</row>
	<row>
		<field name="help_topic_id">135</field>
		<field name="help_keyword_id">451</field>
	</row>
	<row>
		<field name="help_topic_id">296</field>
		<field name="help_keyword_id">451</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">451</field>
	</row>
	<row>
		<field name="help_topic_id">497</field>
		<field name="help_keyword_id">452</field>
	</row>
	<row>
		<field name="help_topic_id">27</field>
		<field name="help_keyword_id">453</field>
	</row>
	<row>
		<field name="help_topic_id">22</field>
		<field name="help_keyword_id">454</field>
	</row>
	<row>
		<field name="help_topic_id">265</field>
		<field name="help_keyword_id">454</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">454</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="help_keyword_id">455</field>
	</row>
	<row>
		<field name="help_topic_id">494</field>
		<field name="help_keyword_id">455</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="help_keyword_id">456</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="help_keyword_id">457</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="help_keyword_id">457</field>
	</row>
	<row>
		<field name="help_topic_id">211</field>
		<field name="help_keyword_id">458</field>
	</row>
	<row>
		<field name="help_topic_id">107</field>
		<field name="help_keyword_id">459</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">459</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">459</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="help_keyword_id">460</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="help_keyword_id">461</field>
	</row>
	<row>
		<field name="help_topic_id">480</field>
		<field name="help_keyword_id">461</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="help_keyword_id">462</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="help_keyword_id">462</field>
	</row>
	<row>
		<field name="help_topic_id">39</field>
		<field name="help_keyword_id">463</field>
	</row>
	<row>
		<field name="help_topic_id">59</field>
		<field name="help_keyword_id">463</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="help_keyword_id">463</field>
	</row>
	<row>
		<field name="help_topic_id">266</field>
		<field name="help_keyword_id">463</field>
	</row>
	<row>
		<field name="help_topic_id">271</field>
		<field name="help_keyword_id">463</field>
	</row>
	<row>
		<field name="help_topic_id">210</field>
		<field name="help_keyword_id">464</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">464</field>
	</row>
	<row>
		<field name="help_topic_id">202</field>
		<field name="help_keyword_id">465</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="help_keyword_id">466</field>
	</row>
	</table_data>
	<table_structure name="help_topic">
		<field Field="help_topic_id" Type="int(10) unsigned" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="name" Type="char(64)" Null="NO" Key="UNI" Extra="" Comment="" />
		<field Field="help_category_id" Type="smallint(5) unsigned" Null="NO" Key="" Extra="" Comment="" />
		<field Field="description" Type="text" Null="NO" Key="" Extra="" Comment="" />
		<field Field="example" Type="text" Null="NO" Key="" Extra="" Comment="" />
		<field Field="url" Type="char(128)" Null="NO" Key="" Extra="" Comment="" />
		<key Table="help_topic" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="help_topic_id" Collation="A" Cardinality="510" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="help_topic" Non_unique="0" Key_name="name" Seq_in_index="1" Column_name="name" Collation="A" Cardinality="510" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="help_topic" Engine="MyISAM" Version="10" Row_format="Dynamic" Rows="510" Avg_row_length="899" Data_length="458748" Max_data_length="281474976710655" Index_length="20480" Data_free="0" Create_time="2013-06-14 00:06:54" Update_time="2013-06-14 00:06:56" Collation="utf8_general_ci" Create_options="" Comment="help topics" />
	</table_structure>
	<table_data name="help_topic">
	<row>
		<field name="help_topic_id">0</field>
		<field name="name">MIN</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
MIN([DISTINCT] expr)

Returns the minimum value of expr. MIN() may take a string argument; in
such cases, it returns the minimum string value. See
http://dev.mysql.com/doc/refman/5.5/en/mysql-indexes.html. The DISTINCT
keyword can be used to find the minimum of the distinct values of expr,
however, this produces the same result as omitting DISTINCT.

MIN() returns NULL if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example">mysql&gt; SELECT student_name, MIN(test_score), MAX(test_score)
    -&gt;        FROM student
    -&gt;        GROUP BY student_name;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">1</field>
		<field name="name">JOIN</field>
		<field name="help_category_id">28</field>
		<field name="description">MySQL supports the following JOIN syntaxes for the table_references
part of SELECT statements and multiple-table DELETE and UPDATE
statements:

table_references:
    table_reference [, table_reference] ...

table_reference:
    table_factor
  | join_table

table_factor:
    tbl_name [[AS] alias] [index_hint_list]
  | table_subquery [AS] alias
  | ( table_references )
  | { OJ table_reference LEFT OUTER JOIN table_reference
        ON conditional_expr }

join_table:
    table_reference [INNER | CROSS] JOIN table_factor [join_condition]
  | table_reference STRAIGHT_JOIN table_factor
  | table_reference STRAIGHT_JOIN table_factor ON conditional_expr
  | table_reference {LEFT|RIGHT} [OUTER] JOIN table_reference join_condition
  | table_reference NATURAL [{LEFT|RIGHT} [OUTER]] JOIN table_factor

join_condition:
    ON conditional_expr
  | USING (column_list)

index_hint_list:
    index_hint [, index_hint] ...

index_hint:
    USE {INDEX|KEY}
      [FOR {JOIN|ORDER BY|GROUP BY}] ([index_list])
  | IGNORE {INDEX|KEY}
      [FOR {JOIN|ORDER BY|GROUP BY}] (index_list)
  | FORCE {INDEX|KEY}
      [FOR {JOIN|ORDER BY|GROUP BY}] (index_list)

index_list:
    index_name [, index_name] ...

A table reference is also known as a join expression.

The syntax of table_factor is extended in comparison with the SQL
Standard. The latter accepts only table_reference, not a list of them
inside a pair of parentheses.

This is a conservative extension if we consider each comma in a list of
table_reference items as equivalent to an inner join. For example:

SELECT * FROM t1 LEFT JOIN (t2, t3, t4)
                 ON (t2.a=t1.a AND t3.b=t1.b AND t4.c=t1.c)

is equivalent to:

SELECT * FROM t1 LEFT JOIN (t2 CROSS JOIN t3 CROSS JOIN t4)
                 ON (t2.a=t1.a AND t3.b=t1.b AND t4.c=t1.c)

In MySQL, JOIN, CROSS JOIN, and INNER JOIN are syntactic equivalents
(they can replace each other). In standard SQL, they are not
equivalent. INNER JOIN is used with an ON clause, CROSS JOIN is used
otherwise.

In general, parentheses can be ignored in join expressions containing
only inner join operations. MySQL also supports nested joins (see
http://dev.mysql.com/doc/refman/5.5/en/nested-join-optimization.html).

Index hints can be specified to affect how the MySQL optimizer makes
use of indexes. For more information, see
http://dev.mysql.com/doc/refman/5.5/en/index-hints.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/join.html

</field>
		<field name="example">SELECT left_tbl.*
  FROM left_tbl LEFT JOIN right_tbl ON left_tbl.id = right_tbl.id
  WHERE right_tbl.id IS NULL;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/join.html</field>
	</row>
	<row>
		<field name="help_topic_id">2</field>
		<field name="name">HEX</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
HEX(str), HEX(N)

For a string argument str, HEX() returns a hexadecimal string
representation of str where each character in str is converted to two
hexadecimal digits. The inverse of this operation is performed by the
UNHEX() function.

For a numeric argument N, HEX() returns a hexadecimal string
representation of the value of N treated as a longlong (BIGINT) number.
This is equivalent to CONV(N,10,16). The inverse of this operation is
performed by CONV(HEX(N),16,10).

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT 0x616263, HEX('abc'), UNHEX(HEX('abc'));
        -&gt; 'abc', 616263, 'abc'
mysql&gt; SELECT HEX(255), CONV(HEX(255),16,10);
        -&gt; 'FF', 255
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">3</field>
		<field name="name">REPLACE</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
REPLACE [LOW_PRIORITY | DELAYED]
    [INTO] tbl_name [(col_name,...)]
    {VALUES | VALUE} ({expr | DEFAULT},...),(...),...

Or:

REPLACE [LOW_PRIORITY | DELAYED]
    [INTO] tbl_name
    SET col_name={expr | DEFAULT}, ...

Or:

REPLACE [LOW_PRIORITY | DELAYED]
    [INTO] tbl_name [(col_name,...)]
    SELECT ...

REPLACE works exactly like INSERT, except that if an old row in the
table has the same value as a new row for a PRIMARY KEY or a UNIQUE
index, the old row is deleted before the new row is inserted. See [HELP
INSERT].

REPLACE is a MySQL extension to the SQL standard. It either inserts, or
deletes and inserts. For another MySQL extension to standard SQL---that
either inserts or updates---see
http://dev.mysql.com/doc/refman/5.5/en/insert-on-duplicate.html.

Note that unless the table has a PRIMARY KEY or UNIQUE index, using a
REPLACE statement makes no sense. It becomes equivalent to INSERT,
because there is no index to be used to determine whether a new row
duplicates another.

Values for all columns are taken from the values specified in the
REPLACE statement. Any missing columns are set to their default values,
just as happens for INSERT. You cannot refer to values from the current
row and use them in the new row. If you use an assignment such as SET
col_name = col_name + 1, the reference to the column name on the right
hand side is treated as DEFAULT(col_name), so the assignment is
equivalent to SET col_name = DEFAULT(col_name) + 1.

To use REPLACE, you must have both the INSERT and DELETE privileges for
the table.

URL: http://dev.mysql.com/doc/refman/5.5/en/replace.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/replace.html</field>
	</row>
	<row>
		<field name="help_topic_id">4</field>
		<field name="name">CONTAINS</field>
		<field name="help_category_id">31</field>
		<field name="description">Contains(g1,g2)

Returns 1 or 0 to indicate whether g1 completely contains g2. This
tests the opposite relationship as Within().

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">5</field>
		<field name="name">SRID</field>
		<field name="help_category_id">37</field>
		<field name="description">SRID(g)

Returns an integer indicating the Spatial Reference System ID for the
geometry value g.

In MySQL, the SRID value is just an integer associated with the
geometry value. All calculations are done assuming Euclidean (planar)
geometry.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SELECT SRID(GeomFromText('LineString(1 1,2 2)',101));
+-----------------------------------------------+
| SRID(GeomFromText('LineString(1 1,2 2)',101)) |
+-----------------------------------------------+
|                                           101 |
+-----------------------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">6</field>
		<field name="name">CURRENT_TIMESTAMP</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
CURRENT_TIMESTAMP, CURRENT_TIMESTAMP()

CURRENT_TIMESTAMP and CURRENT_TIMESTAMP() are synonyms for NOW().

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">7</field>
		<field name="name">SHOW CONTRIBUTORS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW CONTRIBUTORS

The SHOW CONTRIBUTORS statement displays information about the people
who contribute to MySQL source or to causes that we support. For each
contributor, it displays Name, Location, and Comment values.

This statement is deprecated as of MySQL 5.5.29 and is removed in MySQL
5.6.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-contributors.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-contributors.html</field>
	</row>
	<row>
		<field name="help_topic_id">8</field>
		<field name="name">VARIANCE</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
VARIANCE(expr)

Returns the population standard variance of expr. This is an extension
to standard SQL. The standard SQL function VAR_POP() can be used
instead.

VARIANCE() returns NULL if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">9</field>
		<field name="name">DROP SERVER</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
DROP SERVER [ IF EXISTS ] server_name

Drops the server definition for the server named server_name. The
corresponding row in the mysql.servers table is deleted. This statement
requires the SUPER privilege.

Dropping a server for a table does not affect any FEDERATED tables that
used this connection information when they were created. See [HELP
CREATE SERVER].

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-server.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-server.html</field>
	</row>
	<row>
		<field name="help_topic_id">10</field>
		<field name="name">SHOW AUTHORS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW AUTHORS

The SHOW AUTHORS statement displays information about the people who
work on MySQL. For each author, it displays Name, Location, and Comment
values.

This statement is deprecated as of MySQL 5.5.29 and is removed in MySQL
5.6.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-authors.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-authors.html</field>
	</row>
	<row>
		<field name="help_topic_id">11</field>
		<field name="name">VAR_SAMP</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
VAR_SAMP(expr)

Returns the sample variance of expr. That is, the denominator is the
number of rows minus one.

VAR_SAMP() returns NULL if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">12</field>
		<field name="name">CONCAT</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
CONCAT(str1,str2,...)

Returns the string that results from concatenating the arguments. May
have one or more arguments. If all arguments are nonbinary strings, the
result is a nonbinary string. If the arguments include any binary
strings, the result is a binary string. A numeric argument is converted
to its equivalent string form. This is a nonbinary string as of MySQL
5.5.3. Before 5.5.3, it is a binary string; to to avoid that and
produce a nonbinary string, you can use an explicit type cast, as in
this example:

SELECT CONCAT(CAST(int_col AS CHAR), char_col);

CONCAT() returns NULL if any argument is NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT CONCAT('My', 'S', 'QL');
        -&gt; 'MySQL'
mysql&gt; SELECT CONCAT('My', NULL, 'QL');
        -&gt; NULL
mysql&gt; SELECT CONCAT(14.3);
        -&gt; '14.3'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">13</field>
		<field name="name">GEOMETRY HIERARCHY</field>
		<field name="help_category_id">35</field>
		<field name="description">Geometry is the base class. It is an abstract class. The instantiable
subclasses of Geometry are restricted to zero-, one-, and
two-dimensional geometric objects that exist in two-dimensional
coordinate space. All instantiable geometry classes are defined so that
valid instances of a geometry class are topologically closed (that is,
all defined geometries include their boundary).

The base Geometry class has subclasses for Point, Curve, Surface, and
GeometryCollection:

o Point represents zero-dimensional objects.

o Curve represents one-dimensional objects, and has subclass
  LineString, with sub-subclasses Line and LinearRing.

o Surface is designed for two-dimensional objects and has subclass
  Polygon.

o GeometryCollection has specialized zero-, one-, and two-dimensional
  collection classes named MultiPoint, MultiLineString, and
  MultiPolygon for modeling geometries corresponding to collections of
  Points, LineStrings, and Polygons, respectively. MultiCurve and
  MultiSurface are introduced as abstract superclasses that generalize
  the collection interfaces to handle Curves and Surfaces.

Geometry, Curve, Surface, MultiCurve, and MultiSurface are defined as
noninstantiable classes. They define a common set of methods for their
subclasses and are included for extensibility.

Point, LineString, Polygon, GeometryCollection, MultiPoint,
MultiLineString, and MultiPolygon are instantiable classes.

URL: http://dev.mysql.com/doc/refman/5.5/en/gis-geometry-class-hierarchy.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/gis-geometry-class-hierarchy.html</field>
	</row>
	<row>
		<field name="help_topic_id">14</field>
		<field name="name">CHAR FUNCTION</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
CHAR(N,... [USING charset_name])

CHAR() interprets each argument N as an integer and returns a string
consisting of the characters given by the code values of those
integers. NULL values are skipped.
By default, CHAR() returns a binary string. To produce a string in a
given character set, use the optional USING clause:

mysql&gt; SELECT CHARSET(CHAR(0x65)), CHARSET(CHAR(0x65 USING utf8));
+---------------------+--------------------------------+
| CHARSET(CHAR(0x65)) | CHARSET(CHAR(0x65 USING utf8)) |
+---------------------+--------------------------------+
| binary              | utf8                           |
+---------------------+--------------------------------+

If USING is given and the result string is illegal for the given
character set, a warning is issued. Also, if strict SQL mode is
enabled, the result from CHAR() becomes NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT CHAR(77,121,83,81,'76');
        -&gt; 'MySQL'
mysql&gt; SELECT CHAR(77,77.3,'77.3');
        -&gt; 'MMM'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">15</field>
		<field name="name">DATETIME</field>
		<field name="help_category_id">23</field>
		<field name="description">DATETIME

A date and time combination. The supported range is '1000-01-01
00:00:00' to '9999-12-31 23:59:59'. MySQL displays DATETIME values in
'YYYY-MM-DD HH:MM:SS' format, but permits assignment of values to
DATETIME columns using either strings or numbers.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">16</field>
		<field name="name">SHOW CREATE TRIGGER</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW CREATE TRIGGER trigger_name

This statement shows a CREATE TRIGGER statement that creates the given
trigger.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-create-trigger.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-create-trigger.html</field>
	</row>
	<row>
		<field name="help_topic_id">17</field>
		<field name="name">SHOW CREATE PROCEDURE</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW CREATE PROCEDURE proc_name

This statement is a MySQL extension. It returns the exact string that
can be used to re-create the named stored procedure. A similar
statement, SHOW CREATE FUNCTION, displays information about stored
functions (see [HELP SHOW CREATE FUNCTION]).

Both statements require that you be the owner of the routine or have
SELECT access to the mysql.proc table. If you do not have privileges
for the routine itself, the value displayed for the Create Procedure or
Create Function field will be NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-create-procedure.html

</field>
		<field name="example">mysql&gt; SHOW CREATE PROCEDURE test.simpleproc\G
*************************** 1. row ***************************
           Procedure: simpleproc
            sql_mode:
    Create Procedure: CREATE PROCEDURE `simpleproc`(OUT param1 INT)
                      BEGIN
                      SELECT COUNT(*) INTO param1 FROM t;
                      END
character_set_client: latin1
collation_connection: latin1_swedish_ci
  Database Collation: latin1_swedish_ci

mysql&gt; SHOW CREATE FUNCTION test.hello\G
*************************** 1. row ***************************
            Function: hello
            sql_mode:
     Create Function: CREATE FUNCTION `hello`(s CHAR(20))
                      RETURNS CHAR(50)
                      RETURN CONCAT('Hello, ',s,'!')
character_set_client: latin1
collation_connection: latin1_swedish_ci
  Database Collation: latin1_swedish_ci
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-create-procedure.html</field>
	</row>
	<row>
		<field name="help_topic_id">18</field>
		<field name="name">OPEN</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
OPEN cursor_name

This statement opens a previously declared cursor. For an example, see
http://dev.mysql.com/doc/refman/5.5/en/cursors.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/open.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/open.html</field>
	</row>
	<row>
		<field name="help_topic_id">19</field>
		<field name="name">INTEGER</field>
		<field name="help_category_id">23</field>
		<field name="description">INTEGER[(M)] [UNSIGNED] [ZEROFILL]

This type is a synonym for INT.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">20</field>
		<field name="name">LOWER</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
LOWER(str)

Returns the string str with all characters changed to lowercase
according to the current character set mapping. The default is latin1
(cp1252 West European).

mysql&gt; SELECT LOWER('QUADRATICALLY');
        -&gt; 'quadratically'

LOWER() (and UPPER()) are ineffective when applied to binary strings
(BINARY, VARBINARY, BLOB). To perform lettercase conversion, convert
the string to a nonbinary string:

mysql&gt; SET @str = BINARY 'New York';
mysql&gt; SELECT LOWER(@str), LOWER(CONVERT(@str USING latin1));
+-------------+-----------------------------------+
| LOWER(@str) | LOWER(CONVERT(@str USING latin1)) |
+-------------+-----------------------------------+
| New York    | new york                          |
+-------------+-----------------------------------+

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">21</field>
		<field name="name">SHOW COLUMNS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW [FULL] COLUMNS {FROM | IN} tbl_name [{FROM | IN} db_name]
    [LIKE 'pattern' | WHERE expr]

SHOW COLUMNS displays information about the columns in a given table.
It also works for views. The LIKE clause, if present, indicates which
column names to match. The WHERE clause can be given to select rows
using more general conditions, as discussed in
http://dev.mysql.com/doc/refman/5.5/en/extended-show.html.

SHOW COLUMNS displays information only for those columns for which you
have some privilege.

mysql&gt; SHOW COLUMNS FROM City;
+------------+----------+------+-----+---------+----------------+
| Field      | Type     | Null | Key | Default | Extra          |
+------------+----------+------+-----+---------+----------------+
| Id         | int(11)  | NO   | PRI | NULL    | auto_increment |
| Name       | char(35) | NO   |     |         |                |
| Country    | char(3)  | NO   | UNI |         |                |
| District   | char(20) | YES  | MUL |         |                |
| Population | int(11)  | NO   |     | 0       |                |
+------------+----------+------+-----+---------+----------------+
5 rows in set (0.00 sec)

If the data types differ from what you expect them to be based on a
CREATE TABLE statement, note that MySQL sometimes changes data types
when you create or alter a table. The conditions under which this
occurs are described in
http://dev.mysql.com/doc/refman/5.5/en/silent-column-changes.html.

The FULL keyword causes the output to include the column collation and
comments, as well as the privileges you have for each column.

You can use db_name.tbl_name as an alternative to the tbl_name FROM
db_name syntax. In other words, these two statements are equivalent:

mysql&gt; SHOW COLUMNS FROM mytable FROM mydb;
mysql&gt; SHOW COLUMNS FROM mydb.mytable;

SHOW COLUMNS displays the following values for each table column:

Field indicates the column name.

Type indicates the column data type.

Collation indicates the collation for nonbinary string columns, or NULL
for other columns. This value is displayed only if you use the FULL
keyword.

The Null field contains YES if NULL values can be stored in the column,
NO if not.

The Key field indicates whether the column is indexed:

o If Key is empty, the column either is not indexed or is indexed only
  as a secondary column in a multiple-column, nonunique index.

o If Key is PRI, the column is a PRIMARY KEY or is one of the columns
  in a multiple-column PRIMARY KEY.

o If Key is UNI, the column is the first column of a UNIQUE index. (A
  UNIQUE index permits multiple NULL values, but you can tell whether
  the column permits NULL by checking the Null field.)

o If Key is MUL, the column is the first column of a nonunique index in
  which multiple occurrences of a given value are permitted within the
  column.

If more than one of the Key values applies to a given column of a
table, Key displays the one with the highest priority, in the order
PRI, UNI, MUL.

A UNIQUE index may be displayed as PRI if it cannot contain NULL values
and there is no PRIMARY KEY in the table. A UNIQUE index may display as
MUL if several columns form a composite UNIQUE index; although the
combination of the columns is unique, each column can still hold
multiple occurrences of a given value.

The Default field indicates the default value that is assigned to the
column. This is NULL if the column has an explicit default of NULL, or
if the column definition has no DEFAULT clause.

The Extra field contains any additional information that is available
about a given column. The value is nonempty in these cases:
auto_increment for columns that have the AUTO_INCREMENT attribute; on
update CURRENT_TIMESTAMP for TIMESTAMP columns that have the ON UPDATE
CURRENT_TIMESTAMP attribute.

Privileges indicates the privileges you have for the column. This value
is displayed only if you use the FULL keyword.

Comment indicates any comment the column has. This value is displayed
only if you use the FULL keyword.

SHOW FIELDS is a synonym for SHOW COLUMNS. You can also list a table's
columns with the mysqlshow db_name tbl_name command.

The DESCRIBE statement provides information similar to SHOW COLUMNS.
See http://dev.mysql.com/doc/refman/5.5/en/describe.html.

The SHOW CREATE TABLE, SHOW TABLE STATUS, and SHOW INDEX statements
also provide information about tables. See [HELP SHOW].

URL: http://dev.mysql.com/doc/refman/5.5/en/show-columns.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-columns.html</field>
	</row>
	<row>
		<field name="help_topic_id">22</field>
		<field name="name">CREATE TRIGGER</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
CREATE
    [DEFINER = { user | CURRENT_USER }]
    TRIGGER trigger_name trigger_time trigger_event
    ON tbl_name FOR EACH ROW trigger_body

This statement creates a new trigger. A trigger is a named database
object that is associated with a table, and that activates when a
particular event occurs for the table. The trigger becomes associated
with the table named tbl_name, which must refer to a permanent table.
You cannot associate a trigger with a TEMPORARY table or a view.

CREATE TRIGGER requires the TRIGGER privilege for the table associated
with the trigger. The statement might also require the SUPER privilege,
depending on the DEFINER value, as described later in this section. If
binary logging is enabled, CREATE TRIGGER might require the SUPER
privilege, as described in
http://dev.mysql.com/doc/refman/5.5/en/stored-programs-logging.html.

The DEFINER clause determines the security context to be used when
checking access privileges at trigger activation time. See later in
this section for more information.

trigger_time is the trigger action time. It can be BEFORE or AFTER to
indicate that the trigger activates before or after each row to be
modified.

trigger_event indicates the kind of statement that activates the
trigger. The trigger_event can be one of the following:

o INSERT: The trigger is activated whenever a new row is inserted into
  the table; for example, through INSERT, LOAD DATA, and REPLACE
  statements.

o UPDATE: The trigger is activated whenever a row is modified; for
  example, through UPDATE statements.

o DELETE: The trigger is activated whenever a row is deleted from the
  table; for example, through DELETE and REPLACE statements. However,
  DROP TABLE and TRUNCATE TABLE statements on the table do not activate
  this trigger, because they do not use DELETE. Dropping a partition
  does not activate DELETE triggers, either. See [HELP TRUNCATE TABLE].

URL: http://dev.mysql.com/doc/refman/5.5/en/create-trigger.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-trigger.html</field>
	</row>
	<row>
		<field name="help_topic_id">23</field>
		<field name="name">MONTH</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
MONTH(date)

Returns the month for date, in the range 1 to 12 for January to
December, or 0 for dates such as '0000-00-00' or '2008-00-00' that have
a zero month part.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT MONTH('2008-02-03');
        -&gt; 2
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">24</field>
		<field name="name">TINYINT</field>
		<field name="help_category_id">23</field>
		<field name="description">TINYINT[(M)] [UNSIGNED] [ZEROFILL]

A very small integer. The signed range is -128 to 127. The unsigned
range is 0 to 255.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">25</field>
		<field name="name">SHOW TRIGGERS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW TRIGGERS [{FROM | IN} db_name]
    [LIKE 'pattern' | WHERE expr]

SHOW TRIGGERS lists the triggers currently defined for tables in a
database (the default database unless a FROM clause is given). This
statement returns results only for databases and tables for which you
have the TRIGGER privilege. The LIKE clause, if present, indicates
which table names to match and causes the statement to display triggers
for those tables. The WHERE clause can be given to select rows using
more general conditions, as discussed in
http://dev.mysql.com/doc/refman/5.5/en/extended-show.html.

For the trigger ins_sum as defined in
http://dev.mysql.com/doc/refman/5.5/en/triggers.html, the output of
this statement is as shown here:

mysql&gt; SHOW TRIGGERS LIKE 'acc%'\G
*************************** 1. row ***************************
             Trigger: ins_sum
               Event: INSERT
               Table: account
           Statement: SET @sum = @sum + NEW.amount
              Timing: BEFORE
             Created: NULL
            sql_mode:
             Definer: myname@localhost
character_set_client: latin1
collation_connection: latin1_swedish_ci
  Database Collation: latin1_swedish_ci

character_set_client is the session value of the character_set_client
system variable when the trigger was created. collation_connection is
the session value of the collation_connection system variable when the
trigger was created. Database Collation is the collation of the
database with which the trigger is associated.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-triggers.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-triggers.html</field>
	</row>
	<row>
		<field name="help_topic_id">26</field>
		<field name="name">MASTER_POS_WAIT</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
MASTER_POS_WAIT(log_name,log_pos[,timeout])

This function is useful for control of master/slave synchronization. It
blocks until the slave has read and applied all updates up to the
specified position in the master log. The return value is the number of
log events the slave had to wait for to advance to the specified
position. The function returns NULL if the slave SQL thread is not
started, the slave's master information is not initialized, the
arguments are incorrect, or an error occurs. It returns -1 if the
timeout has been exceeded. If the slave SQL thread stops while
MASTER_POS_WAIT() is waiting, the function returns NULL. If the slave
is past the specified position, the function returns immediately.

If a timeout value is specified, MASTER_POS_WAIT() stops waiting when
timeout seconds have elapsed. timeout must be greater than 0; a zero or
negative timeout means no timeout.

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">27</field>
		<field name="name">REGEXP</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
expr REGEXP pat, expr RLIKE pat

Performs a pattern match of a string expression expr against a pattern
pat. The pattern can be an extended regular expression. The syntax for
regular expressions is discussed in
http://dev.mysql.com/doc/refman/5.5/en/regexp.html. Returns 1 if expr
matches pat; otherwise it returns 0. If either expr or pat is NULL, the
result is NULL. RLIKE is a synonym for REGEXP, provided for mSQL
compatibility.

The pattern need not be a literal string. For example, it can be
specified as a string expression or table column.

*Note*: Because MySQL uses the C escape syntax in strings (for example,
&quot;\n&quot; to represent the newline character), you must double any &quot;\&quot; that
you use in your REGEXP strings.

REGEXP is not case sensitive, except when used with binary strings.

URL: http://dev.mysql.com/doc/refman/5.5/en/regexp.html

</field>
		<field name="example">mysql&gt; SELECT 'Monty!' REGEXP '.*';
        -&gt; 1
mysql&gt; SELECT 'new*\n*line' REGEXP 'new\\*.\\*line';
        -&gt; 1
mysql&gt; SELECT 'a' REGEXP 'A', 'a' REGEXP BINARY 'A';
        -&gt; 1  0
mysql&gt; SELECT 'a' REGEXP '^[a-d]';
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/regexp.html</field>
	</row>
	<row>
		<field name="help_topic_id">28</field>
		<field name="name">IF STATEMENT</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
IF search_condition THEN statement_list
    [ELSEIF search_condition THEN statement_list] ...
    [ELSE statement_list]
END IF

The IF statement for stored programs implements a basic conditional
construct.

*Note*: There is also an IF() function, which differs from the IF
statement described here. See
http://dev.mysql.com/doc/refman/5.5/en/control-flow-functions.html. The
IF statement can have THEN, ELSE, and ELSEIF clauses, and it is
terminated with END IF.

If the search_condition evaluates to true, the corresponding THEN or
ELSEIF clause statement_list executes. If no search_condition matches,
the ELSE clause statement_list executes.

Each statement_list consists of one or more SQL statements; an empty
statement_list is not permitted.

URL: http://dev.mysql.com/doc/refman/5.5/en/if.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/if.html</field>
	</row>
	<row>
		<field name="help_topic_id">29</field>
		<field name="name">^</field>
		<field name="help_category_id">20</field>
		<field name="description">Syntax:
^

Bitwise XOR:

URL: http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html

</field>
		<field name="example">mysql&gt; SELECT 1 ^ 1;
        -&gt; 0
mysql&gt; SELECT 1 ^ 0;
        -&gt; 1
mysql&gt; SELECT 11 ^ 3;
        -&gt; 8
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">30</field>
		<field name="name">DROP VIEW</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
DROP VIEW [IF EXISTS]
    view_name [, view_name] ...
    [RESTRICT | CASCADE]

DROP VIEW removes one or more views. You must have the DROP privilege
for each view. If any of the views named in the argument list do not
exist, MySQL returns an error indicating by name which nonexisting
views it was unable to drop, but it also drops all of the views in the
list that do exist.

The IF EXISTS clause prevents an error from occurring for views that
don't exist. When this clause is given, a NOTE is generated for each
nonexistent view. See [HELP SHOW WARNINGS].

RESTRICT and CASCADE, if given, are parsed and ignored.

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-view.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-view.html</field>
	</row>
	<row>
		<field name="help_topic_id">31</field>
		<field name="name">WITHIN</field>
		<field name="help_category_id">31</field>
		<field name="description">Within(g1,g2)

Returns 1 or 0 to indicate whether g1 is spatially within g2. This
tests the opposite relationship as Contains().

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">32</field>
		<field name="name">WEEK</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
WEEK(date[,mode])

This function returns the week number for date. The two-argument form
of WEEK() enables you to specify whether the week starts on Sunday or
Monday and whether the return value should be in the range from 0 to 53
or from 1 to 53. If the mode argument is omitted, the value of the
default_week_format system variable is used. See
http://dev.mysql.com/doc/refman/5.5/en/server-system-variables.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT WEEK('2008-02-20');
        -&gt; 7
mysql&gt; SELECT WEEK('2008-02-20',0);
        -&gt; 7
mysql&gt; SELECT WEEK('2008-02-20',1);
        -&gt; 8
mysql&gt; SELECT WEEK('2008-12-31',1);
        -&gt; 53
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">33</field>
		<field name="name">SHOW PLUGINS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW PLUGINS

SHOW PLUGINS displays information about server plugins. Plugin
information is also available in the INFORMATION_SCHEMA.PLUGINS table.
See http://dev.mysql.com/doc/refman/5.5/en/plugins-table.html.

Example of SHOW PLUGINS output:

mysql&gt; SHOW PLUGINS\G
*************************** 1. row ***************************
   Name: binlog
 Status: ACTIVE
   Type: STORAGE ENGINE
Library: NULL
License: GPL
*************************** 2. row ***************************
   Name: CSV
 Status: ACTIVE
   Type: STORAGE ENGINE
Library: NULL
License: GPL
*************************** 3. row ***************************
   Name: MEMORY
 Status: ACTIVE
   Type: STORAGE ENGINE
Library: NULL
License: GPL
*************************** 4. row ***************************
   Name: MyISAM
 Status: ACTIVE
   Type: STORAGE ENGINE
Library: NULL
License: GPL
...

URL: http://dev.mysql.com/doc/refman/5.5/en/show-plugins.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-plugins.html</field>
	</row>
	<row>
		<field name="help_topic_id">34</field>
		<field name="name">DROP FUNCTION UDF</field>
		<field name="help_category_id">22</field>
		<field name="description">Syntax:
DROP FUNCTION function_name

This statement drops the user-defined function (UDF) named
function_name.

To drop a function, you must have the DELETE privilege for the mysql
database. This is because DROP FUNCTION removes a row from the
mysql.func system table that records the function's name, type, and
shared library name.

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-function-udf.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-function-udf.html</field>
	</row>
	<row>
		<field name="help_topic_id">35</field>
		<field name="name">PREPARE</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
PREPARE stmt_name FROM preparable_stmt

The PREPARE statement prepares a statement and assigns it a name,
stmt_name, by which to refer to the statement later. Statement names
are not case sensitive. preparable_stmt is either a string literal or a
user variable that contains the text of the statement. The text must
represent a single SQL statement, not multiple statements. Within the
statement, &quot;?&quot; characters can be used as parameter markers to indicate
where data values are to be bound to the query later when you execute
it. The &quot;?&quot; characters should not be enclosed within quotation marks,
even if you intend to bind them to string values. Parameter markers can
be used only where data values should appear, not for SQL keywords,
identifiers, and so forth.

If a prepared statement with the given name already exists, it is
deallocated implicitly before the new statement is prepared. This means
that if the new statement contains an error and cannot be prepared, an
error is returned and no statement with the given name exists.

A prepared statement is executed with EXECUTE and released with
DEALLOCATE PREPARE.

The scope of a prepared statement is the session within which it is
created. Other sessions cannot see it.

URL: http://dev.mysql.com/doc/refman/5.5/en/prepare.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/prepare.html</field>
	</row>
	<row>
		<field name="help_topic_id">36</field>
		<field name="name">LOCK</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
LOCK TABLES
    tbl_name [[AS] alias] lock_type
    [, tbl_name [[AS] alias] lock_type] ...

lock_type:
    READ [LOCAL]
  | [LOW_PRIORITY] WRITE

UNLOCK TABLES

MySQL enables client sessions to acquire table locks explicitly for the
purpose of cooperating with other sessions for access to tables, or to
prevent other sessions from modifying tables during periods when a
session requires exclusive access to them. A session can acquire or
release locks only for itself. One session cannot acquire locks for
another session or release locks held by another session.

Locks may be used to emulate transactions or to get more speed when
updating tables. This is explained in more detail later in this
section.

LOCK TABLES explicitly acquires table locks for the current client
session. Table locks can be acquired for base tables or views. You must
have the LOCK TABLES privilege, and the SELECT privilege for each
object to be locked.

For view locking, LOCK TABLES adds all base tables used in the view to
the set of tables to be locked and locks them automatically. If you
lock a table explicitly with LOCK TABLES, any tables used in triggers
are also locked implicitly, as described in
http://dev.mysql.com/doc/refman/5.5/en/lock-tables-and-triggers.html.

UNLOCK TABLES explicitly releases any table locks held by the current
session. LOCK TABLES implicitly releases any table locks held by the
current session before acquiring new locks.

Another use for UNLOCK TABLES is to release the global read lock
acquired with the FLUSH TABLES WITH READ LOCK statement, which enables
you to lock all tables in all databases. See [HELP FLUSH]. (This is a
very convenient way to get backups if you have a file system such as
Veritas that can take snapshots in time.)

URL: http://dev.mysql.com/doc/refman/5.5/en/lock-tables.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/lock-tables.html</field>
	</row>
	<row>
		<field name="help_topic_id">37</field>
		<field name="name">UPDATEXML</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
UpdateXML(xml_target, xpath_expr, new_xml)

This function replaces a single portion of a given fragment of XML
markup xml_target with a new XML fragment new_xml, and then returns the
changed XML. The portion of xml_target that is replaced matches an
XPath expression xpath_expr supplied by the user. In MySQL 5.5, the
XPath expression can contain at most 127 characters. (This limitation
is lifted in MySQL 5.6.)

If no expression matching xpath_expr is found, or if multiple matches
are found, the function returns the original xml_target XML fragment.
All three arguments should be strings.

URL: http://dev.mysql.com/doc/refman/5.5/en/xml-functions.html

</field>
		<field name="example">mysql&gt; SELECT
    -&gt;   UpdateXML('&lt;a&gt;&lt;b&gt;ccc&lt;/b&gt;&lt;d&gt;&lt;/d&gt;&lt;/a&gt;', '/a', '&lt;e&gt;fff&lt;/e&gt;') AS val1,
    -&gt;   UpdateXML('&lt;a&gt;&lt;b&gt;ccc&lt;/b&gt;&lt;d&gt;&lt;/d&gt;&lt;/a&gt;', '/b', '&lt;e&gt;fff&lt;/e&gt;') AS val2,
    -&gt;   UpdateXML('&lt;a&gt;&lt;b&gt;ccc&lt;/b&gt;&lt;d&gt;&lt;/d&gt;&lt;/a&gt;', '//b', '&lt;e&gt;fff&lt;/e&gt;') AS val3,
    -&gt;   UpdateXML('&lt;a&gt;&lt;b&gt;ccc&lt;/b&gt;&lt;d&gt;&lt;/d&gt;&lt;/a&gt;', '/a/d', '&lt;e&gt;fff&lt;/e&gt;') AS val4,
    -&gt;   UpdateXML('&lt;a&gt;&lt;d&gt;&lt;/d&gt;&lt;b&gt;ccc&lt;/b&gt;&lt;d&gt;&lt;/d&gt;&lt;/a&gt;', '/a/d', '&lt;e&gt;fff&lt;/e&gt;') AS val5
    -&gt; \G

*************************** 1. row ***************************
val1: &lt;e&gt;fff&lt;/e&gt;
val2: &lt;a&gt;&lt;b&gt;ccc&lt;/b&gt;&lt;d&gt;&lt;/d&gt;&lt;/a&gt;
val3: &lt;a&gt;&lt;e&gt;fff&lt;/e&gt;&lt;d&gt;&lt;/d&gt;&lt;/a&gt;
val4: &lt;a&gt;&lt;b&gt;ccc&lt;/b&gt;&lt;e&gt;fff&lt;/e&gt;&lt;/a&gt;
val5: &lt;a&gt;&lt;d&gt;&lt;/d&gt;&lt;b&gt;ccc&lt;/b&gt;&lt;d&gt;&lt;/d&gt;&lt;/a&gt;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/xml-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">38</field>
		<field name="name">RESET SLAVE</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
RESET SLAVE [ALL]

RESET SLAVE makes the slave forget its replication position in the
master's binary log. This statement is meant to be used for a clean
start: It deletes the master.info and relay-log.info files, all the
relay log files, and starts a new relay log file. To use RESET SLAVE,
the slave replication threads must be stopped (use STOP SLAVE if
necessary).

*Note*: All relay log files are deleted, even if they have not been
completely executed by the slave SQL thread. (This is a condition
likely to exist on a replication slave if you have issued a STOP SLAVE
statement or if the slave is highly loaded.)

In MySQL 5.5 (unlike the case in MySQL 5.1 and earlier), RESET SLAVE
does not change any replication connection parameters such as master
host, master port, master user, or master password, which are retained
in memory. This means that START SLAVE can be issued without requiring
a CHANGE MASTER TO statement following RESET SLAVE.

Connection parameters are reset if the slave mysqld is shut down
following RESET SLAVE. In MySQL 5.5.16 and later, you can instead use
RESET SLAVE ALL to reset these connection parameters (Bug #11809016).

If the slave SQL thread was in the middle of replicating temporary
tables when it was stopped, and RESET SLAVE is issued, these replicated
temporary tables are deleted on the slave.

URL: http://dev.mysql.com/doc/refman/5.5/en/reset-slave.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/reset-slave.html</field>
	</row>
	<row>
		<field name="help_topic_id">39</field>
		<field name="name">SHOW BINARY LOGS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW BINARY LOGS
SHOW MASTER LOGS

Lists the binary log files on the server. This statement is used as
part of the procedure described in [HELP PURGE BINARY LOGS], that shows
how to determine which logs can be purged.

mysql&gt; SHOW BINARY LOGS;
+---------------+-----------+
| Log_name      | File_size |
+---------------+-----------+
| binlog.000015 |    724935 |
| binlog.000016 |    733481 |
+---------------+-----------+

URL: http://dev.mysql.com/doc/refman/5.5/en/show-binary-logs.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-binary-logs.html</field>
	</row>
	<row>
		<field name="help_topic_id">40</field>
		<field name="name">POLYGON</field>
		<field name="help_category_id">25</field>
		<field name="description">Polygon(ls1,ls2,...)

Constructs a Polygon value from a number of LineString or WKB
LineString arguments. If any argument does not represent a LinearRing
(that is, not a closed and simple LineString), the return value is
NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">41</field>
		<field name="name">MINUTE</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
MINUTE(time)

Returns the minute for time, in the range 0 to 59.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT MINUTE('2008-02-03 10:05:03');
        -&gt; 5
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">42</field>
		<field name="name">DAY</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
DAY(date)

DAY() is a synonym for DAYOFMONTH().

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">43</field>
		<field name="name">MID</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
MID(str,pos,len)

MID(str,pos,len) is a synonym for SUBSTRING(str,pos,len).

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">44</field>
		<field name="name">UUID</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
UUID()

Returns a Universal Unique Identifier (UUID) generated according to
&quot;DCE 1.1: Remote Procedure Call&quot; (Appendix A) CAE (Common Applications
Environment) Specifications published by The Open Group in October 1997
(Document Number C706,
http://www.opengroup.org/public/pubs/catalog/c706.htm).

A UUID is designed as a number that is globally unique in space and
time. Two calls to UUID() are expected to generate two different
values, even if these calls are performed on two separate computers
that are not connected to each other.

A UUID is a 128-bit number represented by a utf8 string of five
hexadecimal numbers in aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee format:

o The first three numbers are generated from a timestamp.

o The fourth number preserves temporal uniqueness in case the timestamp
  value loses monotonicity (for example, due to daylight saving time).

o The fifth number is an IEEE 802 node number that provides spatial
  uniqueness. A random number is substituted if the latter is not
  available (for example, because the host computer has no Ethernet
  card, or we do not know how to find the hardware address of an
  interface on your operating system). In this case, spatial uniqueness
  cannot be guaranteed. Nevertheless, a collision should have very low
  probability.

  Currently, the MAC address of an interface is taken into account only
  on FreeBSD and Linux. On other operating systems, MySQL uses a
  randomly generated 48-bit number.

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example">mysql&gt; SELECT UUID();
        -&gt; '6ccd780c-baba-1026-9564-0040f4311e29'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">45</field>
		<field name="name">LINESTRING</field>
		<field name="help_category_id">25</field>
		<field name="description">LineString(pt1,pt2,...)

Constructs a LineString value from a number of Point or WKB Point
arguments. If the number of arguments is less than two, the return
value is NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">46</field>
		<field name="name">SLEEP</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
SLEEP(duration)

Sleeps (pauses) for the number of seconds given by the duration
argument, then returns 0. If SLEEP() is interrupted, it returns 1. The
duration may have a fractional part.

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">47</field>
		<field name="name">CONNECTION_ID</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
CONNECTION_ID()

Returns the connection ID (thread ID) for the connection. Every
connection has an ID that is unique among the set of currently
connected clients.

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example">mysql&gt; SELECT CONNECTION_ID();
        -&gt; 23786
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">48</field>
		<field name="name">CREATE LOGFILE GROUP</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
CREATE LOGFILE GROUP logfile_group
    ADD UNDOFILE 'undo_file'
    [INITIAL_SIZE [=] initial_size]
    [UNDO_BUFFER_SIZE [=] undo_buffer_size]
    [REDO_BUFFER_SIZE [=] redo_buffer_size]
    [NODEGROUP [=] nodegroup_id]
    [WAIT]
    [COMMENT [=] comment_text]
    ENGINE [=] engine_name

This statement creates a new log file group named logfile_group having
a single UNDO file named 'undo_file'. A CREATE LOGFILE GROUP statement
has one and only one ADD UNDOFILE clause. For rules covering the naming
of log file groups, see
http://dev.mysql.com/doc/refman/5.5/en/identifiers.html.

*Note*: All MySQL Cluster Disk Data objects share the same namespace.
This means that each Disk Data object must be uniquely named (and not
merely each Disk Data object of a given type). For example, you cannot
have a tablespace and a log file group with the same name, or a
tablespace and a data file with the same name.

In MySQL Cluster NDB 7.2, you can have only one log file group per
Cluster at any given time. (See Bug #16386)

The optional INITIAL_SIZE parameter sets the UNDO file's initial size;
if not specified, it defaults to 128M (128 megabytes). The optional
UNDO_BUFFER_SIZE parameter sets the size used by the UNDO buffer for
the log file group; The default value for UNDO_BUFFER_SIZE is 8M (eight
megabytes); this value cannot exceed the amount of system memory
available. Both of these parameters are specified in bytes. You may
optionally follow either or both of these with a one-letter
abbreviation for an order of magnitude, similar to those used in
my.cnf. Generally, this is one of the letters M (for megabytes) or G
(for gigabytes).

The memory used for both INITIAL_SIZE and UNDO_BUFFER_SIZE comes from
the global pool whose size is determined by the value of the
SharedGlobalMemory data node configuration parameter. This includes any
default value implied for these options by the setting of the
InitialLogFileGroup data node configuration parameter.

The maximum permitted for UNDO_BUFFER_SIZE is 600M.

On 32-bit systems, the maximum supported value for INITIAL_SIZE is 4G.

The minimum permitted value for INITIAL_SIZE is 1M.

The ENGINE option determines the storage engine to be used by this log
file group, with engine_name being the name of the storage engine. In
MySQL 5.5, this must be NDB (or NDBCLUSTER). If ENGINE is not set,
MySQL tries to use the engine specified by the default_storage_engine
server system variable (formerly storage_engine). In any case, if the
engine is not specified as NDB or NDBCLUSTER, the CREATE LOGFILE GROUP
statement appears to succeed but actually fails to create the log file
group, as shown here:

mysql&gt; CREATE LOGFILE GROUP lg1 
    -&gt;     ADD UNDOFILE 'undo.dat' INITIAL_SIZE = 10M;
Query OK, 0 rows affected, 1 warning (0.00 sec)

mysql&gt; SHOW WARNINGS;
+-------+------+------------------------------------------------------------------------------------------------+
| Level | Code | Message                                                                                        |
+-------+------+------------------------------------------------------------------------------------------------+
| Error | 1478 | Table storage engine 'InnoDB' does not support the create option 'TABLESPACE or LOGFILE GROUP' |
+-------+------+------------------------------------------------------------------------------------------------+
1 row in set (0.00 sec)

mysql&gt; DROP LOGFILE GROUP lg1 ENGINE = NDB;              
ERROR 1529 (HY000): Failed to drop LOGFILE GROUP

mysql&gt; CREATE LOGFILE GROUP lg1 
    -&gt;     ADD UNDOFILE 'undo.dat' INITIAL_SIZE = 10M
    -&gt;     ENGINE = NDB;
Query OK, 0 rows affected (2.97 sec)

The fact that the CREATE LOGFILE GROUP statement does not actually
return an error when a non-NDB storage engine is named, but rather
appears to succeed, is a known issue which we hope to address in a
future release of MySQL Cluster.

REDO_BUFFER_SIZE, NODEGROUP, WAIT, and COMMENT are parsed but ignored,
and so have no effect in MySQL 5.5. These options are intended for
future expansion.

When used with ENGINE [=] NDB, a log file group and associated UNDO log
file are created on each Cluster data node. You can verify that the
UNDO files were created and obtain information about them by querying
the INFORMATION_SCHEMA.FILES table. For example:

mysql&gt; SELECT LOGFILE_GROUP_NAME, LOGFILE_GROUP_NUMBER, EXTRA
    -&gt; FROM INFORMATION_SCHEMA.FILES
    -&gt; WHERE FILE_NAME = 'undo_10.dat';
+--------------------+----------------------+----------------+
| LOGFILE_GROUP_NAME | LOGFILE_GROUP_NUMBER | EXTRA          |
+--------------------+----------------------+----------------+
| lg_3               |                   11 | CLUSTER_NODE=3 |
| lg_3               |                   11 | CLUSTER_NODE=4 |
+--------------------+----------------------+----------------+
2 rows in set (0.06 sec)

CREATE LOGFILE GROUP is useful only with Disk Data storage for MySQL
Cluster. See
http://dev.mysql.com/doc/refman/5.5/en/mysql-cluster-disk-data.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/create-logfile-group.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-logfile-group.html</field>
	</row>
	<row>
		<field name="help_topic_id">49</field>
		<field name="name">DELETE</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
Single-table syntax:

DELETE [LOW_PRIORITY] [QUICK] [IGNORE] FROM tbl_name
    [WHERE where_condition]
    [ORDER BY ...]
    [LIMIT row_count]

Multiple-table syntax:

DELETE [LOW_PRIORITY] [QUICK] [IGNORE]
    tbl_name[.*] [, tbl_name[.*]] ...
    FROM table_references
    [WHERE where_condition]

Or:

DELETE [LOW_PRIORITY] [QUICK] [IGNORE]
    FROM tbl_name[.*] [, tbl_name[.*]] ...
    USING table_references
    [WHERE where_condition]

For the single-table syntax, the DELETE statement deletes rows from
tbl_name and returns a count of the number of deleted rows. This count
can be obtained by calling the ROW_COUNT() function (see
http://dev.mysql.com/doc/refman/5.5/en/information-functions.html). The
WHERE clause, if given, specifies the conditions that identify which
rows to delete. With no WHERE clause, all rows are deleted. If the
ORDER BY clause is specified, the rows are deleted in the order that is
specified. The LIMIT clause places a limit on the number of rows that
can be deleted.

For the multiple-table syntax, DELETE deletes from each tbl_name the
rows that satisfy the conditions. In this case, ORDER BY and LIMIT
cannot be used.

where_condition is an expression that evaluates to true for each row to
be deleted. It is specified as described in
http://dev.mysql.com/doc/refman/5.5/en/select.html.

Currently, you cannot delete from a table and select from the same
table in a subquery.

You need the DELETE privilege on a table to delete rows from it. You
need only the SELECT privilege for any columns that are only read, such
as those named in the WHERE clause.

As stated, a DELETE statement with no WHERE clause deletes all rows. A
faster way to do this, when you do not need to know the number of
deleted rows, is to use TRUNCATE TABLE. However, within a transaction
or if you have a lock on the table, TRUNCATE TABLE cannot be used
whereas DELETE can. See [HELP TRUNCATE TABLE], and [HELP LOCK].

URL: http://dev.mysql.com/doc/refman/5.5/en/delete.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/delete.html</field>
	</row>
	<row>
		<field name="help_topic_id">50</field>
		<field name="name">ROUND</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
ROUND(X), ROUND(X,D)

Rounds the argument X to D decimal places. The rounding algorithm
depends on the data type of X. D defaults to 0 if not specified. D can
be negative to cause D digits left of the decimal point of the value X
to become zero.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT ROUND(-1.23);
        -&gt; -1
mysql&gt; SELECT ROUND(-1.58);
        -&gt; -2
mysql&gt; SELECT ROUND(1.58);
        -&gt; 2
mysql&gt; SELECT ROUND(1.298, 1);
        -&gt; 1.3
mysql&gt; SELECT ROUND(1.298, 0);
        -&gt; 1
mysql&gt; SELECT ROUND(23.298, -1);
        -&gt; 20
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">51</field>
		<field name="name">NULLIF</field>
		<field name="help_category_id">7</field>
		<field name="description">Syntax:
NULLIF(expr1,expr2)

Returns NULL if expr1 = expr2 is true, otherwise returns expr1. This is
the same as CASE WHEN expr1 = expr2 THEN NULL ELSE expr1 END.

URL: http://dev.mysql.com/doc/refman/5.5/en/control-flow-functions.html

</field>
		<field name="example">mysql&gt; SELECT NULLIF(1,1);
        -&gt; NULL
mysql&gt; SELECT NULLIF(1,2);
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/control-flow-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">52</field>
		<field name="name">CLOSE</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
CLOSE cursor_name

This statement closes a previously opened cursor. For an example, see
http://dev.mysql.com/doc/refman/5.5/en/cursors.html.

An error occurs if the cursor is not open.

If not closed explicitly, a cursor is closed at the end of the BEGIN
... END block in which it was declared.

URL: http://dev.mysql.com/doc/refman/5.5/en/close.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/close.html</field>
	</row>
	<row>
		<field name="help_topic_id">53</field>
		<field name="name">STOP SLAVE</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
STOP SLAVE [thread_types]

thread_types:
    [thread_type [, thread_type] ... ]

thread_type: IO_THREAD | SQL_THREAD

Stops the slave threads. STOP SLAVE requires the SUPER privilege.
Recommended best practice is to execute STOP SLAVE on the slave before
stopping the slave server (see
http://dev.mysql.com/doc/refman/5.5/en/server-shutdown.html, for more
information).

When using the row-based logging format: You should execute STOP SLAVE
on the slave prior to shutting down the slave server if you are
replicating any tables that use a nontransactional storage engine (see
the Note later in this section). In MySQL 5.5.9 and later, you can also
use STOP SLAVE SQL_THREAD for this purpose.

Like START SLAVE, this statement may be used with the IO_THREAD and
SQL_THREAD options to name the thread or threads to be stopped.

*Note*: In MySQL 5.5, STOP SLAVE waits until the current replication
event group affecting one or more nontransactional tables has finished
executing (if there is any such replication group), or until the user
issues a KILL QUERY or KILL CONNECTION statement. (Bug #319, Bug
#38205)

URL: http://dev.mysql.com/doc/refman/5.5/en/stop-slave.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/stop-slave.html</field>
	</row>
	<row>
		<field name="help_topic_id">54</field>
		<field name="name">TIMEDIFF</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
TIMEDIFF(expr1,expr2)

TIMEDIFF() returns expr1 - expr2 expressed as a time value. expr1 and
expr2 are time or date-and-time expressions, but both must be of the
same type.

The result returned by TIMEDIFF() is limited to the range allowed for
TIME values. Alternatively, you can use either of the functions
TIMESTAMPDIFF() and UNIX_TIMESTAMP(), both of which return integers.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT TIMEDIFF('2000:01:01 00:00:00',
    -&gt;                 '2000:01:01 00:00:00.000001');
        -&gt; '-00:00:00.000001'
mysql&gt; SELECT TIMEDIFF('2008-12-31 23:59:59.000001',
    -&gt;                 '2008-12-30 01:01:01.000002');
        -&gt; '46:58:57.999999'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">55</field>
		<field name="name">REPLACE FUNCTION</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
REPLACE(str,from_str,to_str)

Returns the string str with all occurrences of the string from_str
replaced by the string to_str. REPLACE() performs a case-sensitive
match when searching for from_str.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT REPLACE('www.mysql.com', 'w', 'Ww');
        -&gt; 'WwWwWw.mysql.com'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">56</field>
		<field name="name">USE</field>
		<field name="help_category_id">29</field>
		<field name="description">Syntax:
USE db_name

The USE db_name statement tells MySQL to use the db_name database as
the default (current) database for subsequent statements. The database
remains the default until the end of the session or another USE
statement is issued:

USE db1;
SELECT COUNT(*) FROM mytable;   # selects from db1.mytable
USE db2;
SELECT COUNT(*) FROM mytable;   # selects from db2.mytable

URL: http://dev.mysql.com/doc/refman/5.5/en/use.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/use.html</field>
	</row>
	<row>
		<field name="help_topic_id">57</field>
		<field name="name">LINEFROMTEXT</field>
		<field name="help_category_id">3</field>
		<field name="description">LineFromText(wkt[,srid]), LineStringFromText(wkt[,srid])

Constructs a LINESTRING value using its WKT representation and SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">58</field>
		<field name="name">CASE OPERATOR</field>
		<field name="help_category_id">7</field>
		<field name="description">Syntax:
CASE value WHEN [compare_value] THEN result [WHEN [compare_value] THEN
result ...] [ELSE result] END

CASE WHEN [condition] THEN result [WHEN [condition] THEN result ...]
[ELSE result] END

The first version returns the result where value=compare_value. The
second version returns the result for the first condition that is true.
If there was no matching result value, the result after ELSE is
returned, or NULL if there is no ELSE part.

URL: http://dev.mysql.com/doc/refman/5.5/en/control-flow-functions.html

</field>
		<field name="example">mysql&gt; SELECT CASE 1 WHEN 1 THEN 'one'
    -&gt;     WHEN 2 THEN 'two' ELSE 'more' END;
        -&gt; 'one'
mysql&gt; SELECT CASE WHEN 1&gt;0 THEN 'true' ELSE 'false' END;
        -&gt; 'true'
mysql&gt; SELECT CASE BINARY 'B'
    -&gt;     WHEN 'a' THEN 1 WHEN 'b' THEN 2 END;
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/control-flow-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">59</field>
		<field name="name">SHOW MASTER STATUS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW MASTER STATUS

This statement provides status information about the binary log files
of the master. It requires either the SUPER or REPLICATION CLIENT
privilege.

Example:

mysql&gt; SHOW MASTER STATUS;
+---------------+----------+--------------+------------------+
| File          | Position | Binlog_Do_DB | Binlog_Ignore_DB |
+---------------+----------+--------------+------------------+
| mysql-bin.003 | 73       | test         | manual,mysql     |
+---------------+----------+--------------+------------------+

URL: http://dev.mysql.com/doc/refman/5.5/en/show-master-status.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-master-status.html</field>
	</row>
	<row>
		<field name="help_topic_id">60</field>
		<field name="name">ADDTIME</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
ADDTIME(expr1,expr2)

ADDTIME() adds expr2 to expr1 and returns the result. expr1 is a time
or datetime expression, and expr2 is a time expression.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT ADDTIME('2007-12-31 23:59:59.999999', '1 1:1:1.000002');
        -&gt; '2008-01-02 01:01:01.000001'
mysql&gt; SELECT ADDTIME('01:00:00.999999', '02:00:00.999998');
        -&gt; '03:00:01.999997'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">61</field>
		<field name="name">SPATIAL</field>
		<field name="help_category_id">35</field>
		<field name="description">For MyISAM tables, MySQL can create spatial indexes using syntax
similar to that for creating regular indexes, but extended with the
SPATIAL keyword. Currently, columns in spatial indexes must be declared
NOT NULL. The following examples demonstrate how to create spatial
indexes:

o With CREATE TABLE:

CREATE TABLE geom (g GEOMETRY NOT NULL, SPATIAL INDEX(g)) ENGINE=MyISAM;

o With ALTER TABLE:

ALTER TABLE geom ADD SPATIAL INDEX(g);

o With CREATE INDEX:

CREATE SPATIAL INDEX sp_index ON geom (g);

For MyISAM tables, SPATIAL INDEX creates an R-tree index. For storage
engines that support nonspatial indexing of spatial columns, the engine
creates a B-tree index. A B-tree index on spatial values will be useful
for exact-value lookups, but not for range scans.

For more information on indexing spatial columns, see [HELP CREATE
INDEX].

To drop spatial indexes, use ALTER TABLE or DROP INDEX:

o With ALTER TABLE:

ALTER TABLE geom DROP INDEX g;

o With DROP INDEX:

DROP INDEX sp_index ON geom;

Example: Suppose that a table geom contains more than 32,000
geometries, which are stored in the column g of type GEOMETRY. The
table also has an AUTO_INCREMENT column fid for storing object ID
values.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-indexes.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-indexes.html</field>
	</row>
	<row>
		<field name="help_topic_id">62</field>
		<field name="name">TO_SECONDS</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
TO_SECONDS(expr)

Given a date or datetime expr, returns a the number of seconds since
the year 0. If expr is not a valid date or datetime value, returns
NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT TO_SECONDS(950501);
        -&gt; 62966505600
mysql&gt; SELECT TO_SECONDS('2009-11-29');
        -&gt; 63426672000
mysql&gt; SELECT TO_SECONDS('2009-11-29 13:43:32');
        -&gt; 63426721412
mysql&gt; SELECT TO_SECONDS( NOW() );
        -&gt; 63426721458
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">63</field>
		<field name="name">TIMESTAMPDIFF</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
TIMESTAMPDIFF(unit,datetime_expr1,datetime_expr2)

Returns datetime_expr2 - datetime_expr1, where datetime_expr1 and
datetime_expr2 are date or datetime expressions. One expression may be
a date and the other a datetime; a date value is treated as a datetime
having the time part '00:00:00' where necessary. The unit for the
result (an integer) is given by the unit argument. The legal values for
unit are the same as those listed in the description of the
TIMESTAMPADD() function.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT TIMESTAMPDIFF(MONTH,'2003-02-01','2003-05-01');
        -&gt; 3
mysql&gt; SELECT TIMESTAMPDIFF(YEAR,'2002-05-01','2001-01-01');
        -&gt; -1
mysql&gt; SELECT TIMESTAMPDIFF(MINUTE,'2003-02-01','2003-05-01 12:05:55');
        -&gt; 128885
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">64</field>
		<field name="name">UPPER</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
UPPER(str)

Returns the string str with all characters changed to uppercase
according to the current character set mapping. The default is latin1
(cp1252 West European).

mysql&gt; SELECT UPPER('Hej');
        -&gt; 'HEJ'

See the description of LOWER() for information that also applies to
UPPER(), such as information about how to perform lettercase conversion
of binary strings (BINARY, VARBINARY, BLOB) for which these functions
are ineffective.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">65</field>
		<field name="name">FROM_UNIXTIME</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
FROM_UNIXTIME(unix_timestamp), FROM_UNIXTIME(unix_timestamp,format)

Returns a representation of the unix_timestamp argument as a value in
'YYYY-MM-DD HH:MM:SS' or YYYYMMDDHHMMSS.uuuuuu format, depending on
whether the function is used in a string or numeric context. The value
is expressed in the current time zone. unix_timestamp is an internal
timestamp value such as is produced by the UNIX_TIMESTAMP() function.

If format is given, the result is formatted according to the format
string, which is used the same way as listed in the entry for the
DATE_FORMAT() function.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT FROM_UNIXTIME(1196440219);
        -&gt; '2007-11-30 10:30:19'
mysql&gt; SELECT FROM_UNIXTIME(1196440219) + 0;
        -&gt; 20071130103019.000000
mysql&gt; SELECT FROM_UNIXTIME(UNIX_TIMESTAMP(),
    -&gt;                      '%Y %D %M %h:%i:%s %x');
        -&gt; '2007 30th November 10:30:59 2007'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">66</field>
		<field name="name">MEDIUMBLOB</field>
		<field name="help_category_id">23</field>
		<field name="description">MEDIUMBLOB

A BLOB column with a maximum length of 16,777,215 (224 - 1) bytes. Each
MEDIUMBLOB value is stored using a 3-byte length prefix that indicates
the number of bytes in the value.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">67</field>
		<field name="name">SHA2</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
SHA2(str, hash_length)

Calculates the SHA-2 family of hash functions (SHA-224, SHA-256,
SHA-384, and SHA-512). The first argument is the cleartext string to be
hashed. The second argument indicates the desired bit length of the
result, which must have a value of 224, 256, 384, 512, or 0 (which is
equivalent to 256). If either argument is NULL or the hash length is
not one of the permitted values, the return value is NULL. Otherwise,
the function result is a hash value containing the desired number of
bits. See the notes at the beginning of this section about storing hash
values efficiently.

As of MySQL 5.5.6, the return value is a nonbinary string in the
connection character set. Before 5.5.6, the return value is a binary
string; see the notes at the beginning of this section about using the
value as a nonbinary string.

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example">mysql&gt; SELECT SHA2('abc', 224);
        -&gt; '23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">68</field>
		<field name="name">IFNULL</field>
		<field name="help_category_id">7</field>
		<field name="description">Syntax:
IFNULL(expr1,expr2)

If expr1 is not NULL, IFNULL() returns expr1; otherwise it returns
expr2. IFNULL() returns a numeric or string value, depending on the
context in which it is used.

URL: http://dev.mysql.com/doc/refman/5.5/en/control-flow-functions.html

</field>
		<field name="example">mysql&gt; SELECT IFNULL(1,0);
        -&gt; 1
mysql&gt; SELECT IFNULL(NULL,10);
        -&gt; 10
mysql&gt; SELECT IFNULL(1/0,10);
        -&gt; 10
mysql&gt; SELECT IFNULL(1/0,'yes');
        -&gt; 'yes'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/control-flow-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">69</field>
		<field name="name">SHOW FUNCTION CODE</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW FUNCTION CODE func_name

This statement is similar to SHOW PROCEDURE CODE but for stored
functions. See [HELP SHOW PROCEDURE CODE].

URL: http://dev.mysql.com/doc/refman/5.5/en/show-function-code.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-function-code.html</field>
	</row>
	<row>
		<field name="help_topic_id">70</field>
		<field name="name">SHOW ERRORS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW ERRORS [LIMIT [offset,] row_count]
SHOW COUNT(*) ERRORS

This statement is similar to SHOW WARNINGS, except that it displays
information only for errors, rather than for errors, warnings, and
notes.

The LIMIT clause has the same syntax as for the SELECT statement. See
http://dev.mysql.com/doc/refman/5.5/en/select.html.

The SHOW COUNT(*) ERRORS statement displays the number of errors. You
can also retrieve this number from the error_count variable:

SHOW COUNT(*) ERRORS;
SELECT @@error_count;

SHOW ERRORS and error_count apply only to errors, not warnings or
notes. In other respects, they are similar to SHOW WARNINGS and
warning_count. In particular, SHOW ERRORS cannot display information
for more than max_error_count messages, and error_count can exceed the
value of max_error_count if the number of errors exceeds
max_error_count.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-errors.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-errors.html</field>
	</row>
	<row>
		<field name="help_topic_id">71</field>
		<field name="name">LEAST</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
LEAST(value1,value2,...)

With two or more arguments, returns the smallest (minimum-valued)
argument. The arguments are compared using the following rules:

o If any argument is NULL, the result is NULL. No comparison is needed.

o If the return value is used in an INTEGER context or all arguments
  are integer-valued, they are compared as integers.

o If the return value is used in a REAL context or all arguments are
  real-valued, they are compared as reals.

o If the arguments comprise a mix of numbers and strings, they are
  compared as numbers.

o If any argument is a nonbinary (character) string, the arguments are
  compared as nonbinary strings.

o In all other cases, the arguments are compared as binary strings.

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT LEAST(2,0);
        -&gt; 0
mysql&gt; SELECT LEAST(34.0,3.0,5.0,767.0);
        -&gt; 3.0
mysql&gt; SELECT LEAST('B','A','C');
        -&gt; 'A'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">72</field>
		<field name="name">=</field>
		<field name="help_category_id">19</field>
		<field name="description">=

Equal:

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT 1 = 0;
        -&gt; 0
mysql&gt; SELECT '0' = 0;
        -&gt; 1
mysql&gt; SELECT '0.0' = 0;
        -&gt; 1
mysql&gt; SELECT '0.01' = 0;
        -&gt; 0
mysql&gt; SELECT '.01' = 0.01;
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">73</field>
		<field name="name">REVERSE</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
REVERSE(str)

Returns the string str with the order of the characters reversed.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT REVERSE('abc');
        -&gt; 'cba'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">74</field>
		<field name="name">ISNULL</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
ISNULL(expr)

If expr is NULL, ISNULL() returns 1, otherwise it returns 0.

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT ISNULL(1+1);
        -&gt; 0
mysql&gt; SELECT ISNULL(1/0);
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">75</field>
		<field name="name">BINARY</field>
		<field name="help_category_id">23</field>
		<field name="description">BINARY(M)

The BINARY type is similar to the CHAR type, but stores binary byte
strings rather than nonbinary character strings. M represents the
column length in bytes.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">76</field>
		<field name="name">BLOB DATA TYPE</field>
		<field name="help_category_id">23</field>
		<field name="description">A BLOB is a binary large object that can hold a variable amount of
data. The four BLOB types are TINYBLOB, BLOB, MEDIUMBLOB, and LONGBLOB.
These differ only in the maximum length of the values they can hold.
The four TEXT types are TINYTEXT, TEXT, MEDIUMTEXT, and LONGTEXT. These
correspond to the four BLOB types and have the same maximum lengths and
storage requirements. See
http://dev.mysql.com/doc/refman/5.5/en/storage-requirements.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/blob.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/blob.html</field>
	</row>
	<row>
		<field name="help_topic_id">77</field>
		<field name="name">BOUNDARY</field>
		<field name="help_category_id">37</field>
		<field name="description">Boundary(g)

Returns a geometry that is the closure of the combinatorial boundary of
the geometry value g.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">78</field>
		<field name="name">CREATE USER</field>
		<field name="help_category_id">10</field>
		<field name="description">Syntax:
CREATE USER user_specification
    [, user_specification] ...

user_specification:
    user
    [
        IDENTIFIED BY [PASSWORD] 'password'
      | IDENTIFIED WITH auth_plugin [AS 'auth_string']
    ]

The CREATE USER statement creates new MySQL accounts. To use it, you
must have the global CREATE USER privilege or the INSERT privilege for
the mysql database. For each account, CREATE USER creates a new row in
the mysql.user table and assigns the account no privileges. An error
occurs if the account already exists.

Each account name uses the format described in
http://dev.mysql.com/doc/refman/5.5/en/account-names.html. For example:

CREATE USER 'jeffrey'@'localhost' IDENTIFIED BY 'mypass';

If you specify only the user name part of the account name, a host name
part of '%' is used.

The user specification may indicate how the user should authenticate
when connecting to the server:

o To enable the user to connect with no password (which is insecure),
  include no IDENTIFIED BY clause:

CREATE USER 'jeffrey'@'localhost';

  In this case, the account uses built-in authentication and clients
  must provide no password.

o To assign a password, use IDENTIFIED BY with the literal plaintext
  password value:

CREATE USER 'jeffrey'@'localhost' IDENTIFIED BY 'mypass';

  The account uses built-in authentication and clients must match the
  given password.

o To avoid specifying the plaintext password if you know its hash value
  (the value that PASSWORD() would return for the password), specify
  the hash value preceded by the keyword PASSWORD:

CREATE USER 'jeffrey'@'localhost'
IDENTIFIED BY PASSWORD '*90E462C37378CED12064BB3388827D2BA3A9B689';

  The account uses built-in authentication and clients must match the
  given password.

o To authenticate the account using a specific authentication plugin,
  use IDENTIFIED WITH, where auth_plugin is the plugin name. It can be
  an unquoted name or a quoted string literal. 'auth_string' is an
  optional quoted string literal to pass to the plugin. The plugin
  interprets the meaning of the string, so its format is plugin
  specific. Consult the documentation for a given plugin for
  information about the authentication string values it accepts.

CREATE USER 'jeffrey'@'localhost'
IDENTIFIED WITH my_auth_plugin;

  For connections that use this account, the server invokes the named
  plugin and clients must provide credentials as required for the
  authentication method that the plugin implements. If the server
  cannot find the plugin, either at account-creation time or connect
  time, an error occurs. IDENTIFIED WITH can be used as of MySQL 5.5.7.

The IDENTIFIED BY and IDENTIFIED WITH clauses are mutually exclusive,
so at most one of them can be specified for a given user.

For additional information about setting passwords, see
http://dev.mysql.com/doc/refman/5.5/en/assigning-passwords.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/create-user.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-user.html</field>
	</row>
	<row>
		<field name="help_topic_id">79</field>
		<field name="name">POINT</field>
		<field name="help_category_id">25</field>
		<field name="description">Point(x,y)

Constructs a Point using its coordinates.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">80</field>
		<field name="name">CURRENT_USER</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
CURRENT_USER, CURRENT_USER()

Returns the user name and host name combination for the MySQL account
that the server used to authenticate the current client. This account
determines your access privileges. The return value is a string in the
utf8 character set.

The value of CURRENT_USER() can differ from the value of USER().

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example">mysql&gt; SELECT USER();
        -&gt; 'davida@localhost'
mysql&gt; SELECT * FROM mysql.user;
ERROR 1044: Access denied for user ''@'localhost' to
database 'mysql'
mysql&gt; SELECT CURRENT_USER();
        -&gt; '@localhost'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">81</field>
		<field name="name">LCASE</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
LCASE(str)

LCASE() is a synonym for LOWER().

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">82</field>
		<field name="name">&lt;=</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
&lt;=

Less than or equal:

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT 0.1 &lt;= 2;
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">83</field>
		<field name="name">SHOW PROFILES</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW PROFILES

The SHOW PROFILES statement, together with SHOW PROFILE, displays
profiling information that indicates resource usage for statements
executed during the course of the current session. For more
information, see [HELP SHOW PROFILE].

URL: http://dev.mysql.com/doc/refman/5.5/en/show-profiles.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-profiles.html</field>
	</row>
	<row>
		<field name="help_topic_id">84</field>
		<field name="name">UPDATE</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
Single-table syntax:

UPDATE [LOW_PRIORITY] [IGNORE] table_reference
    SET col_name1={expr1|DEFAULT} [, col_name2={expr2|DEFAULT}] ...
    [WHERE where_condition]
    [ORDER BY ...]
    [LIMIT row_count]

Multiple-table syntax:

UPDATE [LOW_PRIORITY] [IGNORE] table_references
    SET col_name1={expr1|DEFAULT} [, col_name2={expr2|DEFAULT}] ...
    [WHERE where_condition]

For the single-table syntax, the UPDATE statement updates columns of
existing rows in the named table with new values. The SET clause
indicates which columns to modify and the values they should be given.
Each value can be given as an expression, or the keyword DEFAULT to set
a column explicitly to its default value. The WHERE clause, if given,
specifies the conditions that identify which rows to update. With no
WHERE clause, all rows are updated. If the ORDER BY clause is
specified, the rows are updated in the order that is specified. The
LIMIT clause places a limit on the number of rows that can be updated.

For the multiple-table syntax, UPDATE updates rows in each table named
in table_references that satisfy the conditions. In this case, ORDER BY
and LIMIT cannot be used.

where_condition is an expression that evaluates to true for each row to
be updated. For expression syntax, see
http://dev.mysql.com/doc/refman/5.5/en/expressions.html.

table_references and where_condition are is specified as described in
http://dev.mysql.com/doc/refman/5.5/en/select.html.

You need the UPDATE privilege only for columns referenced in an UPDATE
that are actually updated. You need only the SELECT privilege for any
columns that are read but not modified.

The UPDATE statement supports the following modifiers:

o With the LOW_PRIORITY keyword, execution of the UPDATE is delayed
  until no other clients are reading from the table. This affects only
  storage engines that use only table-level locking (such as MyISAM,
  MEMORY, and MERGE).

o With the IGNORE keyword, the update statement does not abort even if
  errors occur during the update. Rows for which duplicate-key
  conflicts occur are not updated. Rows for which columns are updated
  to values that would cause data conversion errors are updated to the
  closest valid values instead.

URL: http://dev.mysql.com/doc/refman/5.5/en/update.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/update.html</field>
	</row>
	<row>
		<field name="help_topic_id">85</field>
		<field name="name">IS NOT NULL</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
IS NOT NULL

Tests whether a value is not NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT 1 IS NOT NULL, 0 IS NOT NULL, NULL IS NOT NULL;
        -&gt; 1, 1, 0
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">86</field>
		<field name="name">CASE STATEMENT</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
CASE case_value
    WHEN when_value THEN statement_list
    [WHEN when_value THEN statement_list] ...
    [ELSE statement_list]
END CASE

Or:

CASE
    WHEN search_condition THEN statement_list
    [WHEN search_condition THEN statement_list] ...
    [ELSE statement_list]
END CASE

The CASE statement for stored programs implements a complex conditional
construct.

*Note*: There is also a CASE expression, which differs from the CASE
statement described here. See
http://dev.mysql.com/doc/refman/5.5/en/control-flow-functions.html. The
CASE statement cannot have an ELSE NULL clause, and it is terminated
with END CASE instead of END.

For the first syntax, case_value is an expression. This value is
compared to the when_value expression in each WHEN clause until one of
them is equal. When an equal when_value is found, the corresponding
THEN clause statement_list executes. If no when_value is equal, the
ELSE clause statement_list executes, if there is one.

This syntax cannot be used to test for equality with NULL because NULL
= NULL is false. See
http://dev.mysql.com/doc/refman/5.5/en/working-with-null.html.

For the second syntax, each WHEN clause search_condition expression is
evaluated until one is true, at which point its corresponding THEN
clause statement_list executes. If no search_condition is equal, the
ELSE clause statement_list executes, if there is one.

If no when_value or search_condition matches the value tested and the
CASE statement contains no ELSE clause, a Case not found for CASE
statement error results.

Each statement_list consists of one or more SQL statements; an empty
statement_list is not permitted.

To handle situations where no value is matched by any WHEN clause, use
an ELSE containing an empty BEGIN ... END block, as shown in this
example. (The indentation used here in the ELSE clause is for purposes
of clarity only, and is not otherwise significant.)

DELIMITER |

CREATE PROCEDURE p()
  BEGIN
    DECLARE v INT DEFAULT 1;

    CASE v
      WHEN 2 THEN SELECT v;
      WHEN 3 THEN SELECT 0;
      ELSE
        BEGIN
        END;
    END CASE;
  END;
  |

URL: http://dev.mysql.com/doc/refman/5.5/en/case.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/case.html</field>
	</row>
	<row>
		<field name="help_topic_id">87</field>
		<field name="name">EXECUTE STATEMENT</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
EXECUTE stmt_name
    [USING @var_name [, @var_name] ...]

After preparing a statement with PREPARE, you execute it with an
EXECUTE statement that refers to the prepared statement name. If the
prepared statement contains any parameter markers, you must supply a
USING clause that lists user variables containing the values to be
bound to the parameters. Parameter values can be supplied only by user
variables, and the USING clause must name exactly as many variables as
the number of parameter markers in the statement.

You can execute a given prepared statement multiple times, passing
different variables to it or setting the variables to different values
before each execution.

URL: http://dev.mysql.com/doc/refman/5.5/en/execute.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/execute.html</field>
	</row>
	<row>
		<field name="help_topic_id">88</field>
		<field name="name">DROP INDEX</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
DROP [ONLINE|OFFLINE] INDEX index_name ON tbl_name

DROP INDEX drops the index named index_name from the table tbl_name.
This statement is mapped to an ALTER TABLE statement to drop the index.
See [HELP ALTER TABLE].

To drop a primary key, the index name is always PRIMARY, which must be
specified as a quoted identifier because PRIMARY is a reserved word:

DROP INDEX `PRIMARY` ON t;

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-index.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-index.html</field>
	</row>
	<row>
		<field name="help_topic_id">89</field>
		<field name="name">MATCH AGAINST</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
MATCH (col1,col2,...) AGAINST (expr [search_modifier])

MySQL has support for full-text indexing and searching:

o A full-text index in MySQL is an index of type FULLTEXT.

o Full-text indexes can be used only with MyISAM tables. (In MySQL 5.6
  and up, they can also be used with InnoDB tables.) Full-text indexes
  can be created only for CHAR, VARCHAR, or TEXT columns.

o A FULLTEXT index definition can be given in the CREATE TABLE
  statement when a table is created, or added later using ALTER TABLE
  or CREATE INDEX.

o For large data sets, it is much faster to load your data into a table
  that has no FULLTEXT index and then create the index after that, than
  to load data into a table that has an existing FULLTEXT index.

Full-text searching is performed using MATCH() ... AGAINST syntax.
MATCH() takes a comma-separated list that names the columns to be
searched. AGAINST takes a string to search for, and an optional
modifier that indicates what type of search to perform. The search
string must be a string value that is constant during query evaluation.
This rules out, for example, a table column because that can differ for
each row.

There are three types of full-text searches:

o A natural language search interprets the search string as a phrase in
  natural human language (a phrase in free text). There are no special
  operators. The stopword list applies. In addition, words that are
  present in 50% or more of the rows are considered common and do not
  match.

  Full-text searches are natural language searches if the IN NATURAL
  LANGUAGE MODE modifier is given or if no modifier is given. For more
  information, see
  http://dev.mysql.com/doc/refman/5.5/en/fulltext-natural-language.html
  .

o A boolean search interprets the search string using the rules of a
  special query language. The string contains the words to search for.
  It can also contain operators that specify requirements such that a
  word must be present or absent in matching rows, or that it should be
  weighted higher or lower than usual. Common words such as &quot;some&quot; or
  &quot;then&quot; are stopwords and do not match if present in the search
  string. The IN BOOLEAN MODE modifier specifies a boolean search. For
  more information, see
  http://dev.mysql.com/doc/refman/5.5/en/fulltext-boolean.html.

o A query expansion search is a modification of a natural language
  search. The search string is used to perform a natural language
  search. Then words from the most relevant rows returned by the search
  are added to the search string and the search is done again. The
  query returns the rows from the second search. The IN NATURAL
  LANGUAGE MODE WITH QUERY EXPANSION or WITH QUERY EXPANSION modifier
  specifies a query expansion search. For more information, see
  http://dev.mysql.com/doc/refman/5.5/en/fulltext-query-expansion.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/fulltext-search.html

</field>
		<field name="example">mysql&gt; SELECT id, body, MATCH (title,body) AGAINST
    -&gt; ('Security implications of running MySQL as root'
    -&gt; IN NATURAL LANGUAGE MODE) AS score
    -&gt; FROM articles WHERE MATCH (title,body) AGAINST
    -&gt; ('Security implications of running MySQL as root'
    -&gt; IN NATURAL LANGUAGE MODE);
+----+-------------------------------------+-----------------+
| id | body                                | score           |
+----+-------------------------------------+-----------------+
|  4 | 1. Never run mysqld as root. 2. ... | 1.5219271183014 |
|  6 | When configured properly, MySQL ... | 1.3114095926285 |
+----+-------------------------------------+-----------------+
2 rows in set (0.00 sec)
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/fulltext-search.html</field>
	</row>
	<row>
		<field name="help_topic_id">90</field>
		<field name="name">CREATE EVENT</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
CREATE
    [DEFINER = { user | CURRENT_USER }]
    EVENT
    [IF NOT EXISTS]
    event_name
    ON SCHEDULE schedule
    [ON COMPLETION [NOT] PRESERVE]
    [ENABLE | DISABLE | DISABLE ON SLAVE]
    [COMMENT 'comment']
    DO event_body;

schedule:
    AT timestamp [+ INTERVAL interval] ...
  | EVERY interval
    [STARTS timestamp [+ INTERVAL interval] ...]
    [ENDS timestamp [+ INTERVAL interval] ...]

interval:
    quantity {YEAR | QUARTER | MONTH | DAY | HOUR | MINUTE |
              WEEK | SECOND | YEAR_MONTH | DAY_HOUR | DAY_MINUTE |
              DAY_SECOND | HOUR_MINUTE | HOUR_SECOND | MINUTE_SECOND}

This statement creates and schedules a new event. The event will not
run unless the Event Scheduler is enabled. For information about
checking Event Scheduler status and enabling it if necessary, see
http://dev.mysql.com/doc/refman/5.5/en/events-configuration.html.

CREATE EVENT requires the EVENT privilege for the schema in which the
event is to be created. It might also require the SUPER privilege,
depending on the DEFINER value, as described later in this section.

The minimum requirements for a valid CREATE EVENT statement are as
follows:

o The keywords CREATE EVENT plus an event name, which uniquely
  identifies the event in a database schema.

o An ON SCHEDULE clause, which determines when and how often the event
  executes.

o A DO clause, which contains the SQL statement to be executed by an
  event.

This is an example of a minimal CREATE EVENT statement:

CREATE EVENT myevent
    ON SCHEDULE AT CURRENT_TIMESTAMP + INTERVAL 1 HOUR
    DO
      UPDATE myschema.mytable SET mycol = mycol + 1;

The previous statement creates an event named myevent. This event
executes once---one hour following its creation---by running an SQL
statement that increments the value of the myschema.mytable table's
mycol column by 1.

The event_name must be a valid MySQL identifier with a maximum length
of 64 characters. Event names are not case sensitive, so you cannot
have two events named myevent and MyEvent in the same schema. In
general, the rules governing event names are the same as those for
names of stored routines. See
http://dev.mysql.com/doc/refman/5.5/en/identifiers.html.

An event is associated with a schema. If no schema is indicated as part
of event_name, the default (current) schema is assumed. To create an
event in a specific schema, qualify the event name with a schema using
schema_name.event_name syntax.

URL: http://dev.mysql.com/doc/refman/5.5/en/create-event.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-event.html</field>
	</row>
	<row>
		<field name="help_topic_id">91</field>
		<field name="name">ABS</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
ABS(X)

Returns the absolute value of X.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT ABS(2);
        -&gt; 2
mysql&gt; SELECT ABS(-32);
        -&gt; 32
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">92</field>
		<field name="name">POLYFROMWKB</field>
		<field name="help_category_id">33</field>
		<field name="description">PolyFromWKB(wkb[,srid]), PolygonFromWKB(wkb[,srid])

Constructs a POLYGON value using its WKB representation and SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">93</field>
		<field name="name">NOT LIKE</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
expr NOT LIKE pat [ESCAPE 'escape_char']

This is the same as NOT (expr LIKE pat [ESCAPE 'escape_char']).

URL: http://dev.mysql.com/doc/refman/5.5/en/string-comparison-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-comparison-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">94</field>
		<field name="name">SPACE</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
SPACE(N)

Returns a string consisting of N space characters.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT SPACE(6);
        -&gt; '      '
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">95</field>
		<field name="name">MBR DEFINITION</field>
		<field name="help_category_id">6</field>
		<field name="description">Its MBR (Minimum Bounding Rectangle), or Envelope. This is the bounding
geometry, formed by the minimum and maximum (X,Y) coordinates:

URL: http://dev.mysql.com/doc/refman/5.5/en/gis-class-geometry.html

</field>
		<field name="example">((MINX MINY, MAXX MINY, MAXX MAXY, MINX MAXY, MINX MINY))
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/gis-class-geometry.html</field>
	</row>
	<row>
		<field name="help_topic_id">96</field>
		<field name="name">GEOMETRYCOLLECTION</field>
		<field name="help_category_id">25</field>
		<field name="description">GeometryCollection(g1,g2,...)

Constructs a GeometryCollection.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">97</field>
		<field name="name">MAX</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
MAX([DISTINCT] expr)

Returns the maximum value of expr. MAX() may take a string argument; in
such cases, it returns the maximum string value. See
http://dev.mysql.com/doc/refman/5.5/en/mysql-indexes.html. The DISTINCT
keyword can be used to find the maximum of the distinct values of expr,
however, this produces the same result as omitting DISTINCT.

MAX() returns NULL if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example">mysql&gt; SELECT student_name, MIN(test_score), MAX(test_score)
    -&gt;        FROM student
    -&gt;        GROUP BY student_name;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">98</field>
		<field name="name">CREATE FUNCTION UDF</field>
		<field name="help_category_id">22</field>
		<field name="description">Syntax:
CREATE [AGGREGATE] FUNCTION function_name RETURNS {STRING|INTEGER|REAL|DECIMAL}
    SONAME shared_library_name

A user-defined function (UDF) is a way to extend MySQL with a new
function that works like a native (built-in) MySQL function such as
ABS() or CONCAT().

function_name is the name that should be used in SQL statements to
invoke the function. The RETURNS clause indicates the type of the
function's return value. DECIMAL is a legal value after RETURNS, but
currently DECIMAL functions return string values and should be written
like STRING functions.

shared_library_name is the basename of the shared object file that
contains the code that implements the function. The file must be
located in the plugin directory. This directory is given by the value
of the plugin_dir system variable. For more information, see
http://dev.mysql.com/doc/refman/5.5/en/udf-compiling.html.

To create a function, you must have the INSERT privilege for the mysql
database. This is necessary because CREATE FUNCTION adds a row to the
mysql.func system table that records the function's name, type, and
shared library name. If you do not have this table, you should run the
mysql_upgrade command to create it. See
http://dev.mysql.com/doc/refman/5.5/en/mysql-upgrade.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/create-function-udf.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-function-udf.html</field>
	</row>
	<row>
		<field name="help_topic_id">99</field>
		<field name="name">*</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
*

Multiplication:

URL: http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html

</field>
		<field name="example">mysql&gt; SELECT 3*5;
        -&gt; 15
mysql&gt; SELECT 18014398509481984*18014398509481984.0;
        -&gt; 324518553658426726783156020576256.0
mysql&gt; SELECT 18014398509481984*18014398509481984;
        -&gt; out-of-range error
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">100</field>
		<field name="name">TIMESTAMP</field>
		<field name="help_category_id">23</field>
		<field name="description">TIMESTAMP

A timestamp. The range is '1970-01-01 00:00:01' UTC to '2038-01-19
03:14:07' UTC. TIMESTAMP values are stored as the number of seconds
since the epoch ('1970-01-01 00:00:00' UTC). A TIMESTAMP cannot
represent the value '1970-01-01 00:00:00' because that is equivalent to
0 seconds from the epoch and the value 0 is reserved for representing
'0000-00-00 00:00:00', the &quot;zero&quot; TIMESTAMP value.

Unless specified otherwise, the first TIMESTAMP column in a table is
defined to be automatically set to the date and time of the most recent
modification if not explicitly assigned a value. This makes TIMESTAMP
useful for recording the timestamp of an INSERT or UPDATE operation.
You can also set any TIMESTAMP column to the current date and time by
assigning it a NULL value, unless it has been defined with the NULL
attribute to permit NULL values. The automatic initialization and
updating to the current date and time can be specified using DEFAULT
CURRENT_TIMESTAMP and ON UPDATE CURRENT_TIMESTAMP clauses, as described
in
http://dev.mysql.com/doc/refman/5.5/en/timestamp-initialization.html.

*Note*: The TIMESTAMP format that was used prior to MySQL 4.1 is not
supported in MySQL 5.5; see MySQL 3.23, 4.0, 4.1 Reference Manual for
information regarding the old format.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">101</field>
		<field name="name">DES_DECRYPT</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
DES_DECRYPT(crypt_str[,key_str])

Decrypts a string encrypted with DES_ENCRYPT(). If an error occurs,
this function returns NULL.

This function works only if MySQL has been configured with SSL support.
See http://dev.mysql.com/doc/refman/5.5/en/ssl-connections.html.

If no key_str argument is given, DES_DECRYPT() examines the first byte
of the encrypted string to determine the DES key number that was used
to encrypt the original string, and then reads the key from the DES key
file to decrypt the message. For this to work, the user must have the
SUPER privilege. The key file can be specified with the --des-key-file
server option.

If you pass this function a key_str argument, that string is used as
the key for decrypting the message.

If the crypt_str argument does not appear to be an encrypted string,
MySQL returns the given crypt_str.

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">102</field>
		<field name="name">CACHE INDEX</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
CACHE INDEX
  tbl_index_list [, tbl_index_list] ...
  [PARTITION (partition_list | ALL)]
  IN key_cache_name

tbl_index_list:
  tbl_name [[INDEX|KEY] (index_name[, index_name] ...)]

partition_list:
  partition_name[, partition_name][, ...]

The CACHE INDEX statement assigns table indexes to a specific key
cache. It is used only for MyISAM tables. After the indexes have been
assigned, they can be preloaded into the cache if desired with LOAD
INDEX INTO CACHE.

The following statement assigns indexes from the tables t1, t2, and t3
to the key cache named hot_cache:

mysql&gt; CACHE INDEX t1, t2, t3 IN hot_cache;
+---------+--------------------+----------+----------+
| Table   | Op                 | Msg_type | Msg_text |
+---------+--------------------+----------+----------+
| test.t1 | assign_to_keycache | status   | OK       |
| test.t2 | assign_to_keycache | status   | OK       |
| test.t3 | assign_to_keycache | status   | OK       |
+---------+--------------------+----------+----------+

URL: http://dev.mysql.com/doc/refman/5.5/en/cache-index.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/cache-index.html</field>
	</row>
	<row>
		<field name="help_topic_id">103</field>
		<field name="name">ENDPOINT</field>
		<field name="help_category_id">13</field>
		<field name="description">EndPoint(ls)

Returns the Point that is the endpoint of the LineString value ls.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SET @ls = 'LineString(1 1,2 2,3 3)';
mysql&gt; SELECT AsText(EndPoint(GeomFromText(@ls)));
+-------------------------------------+
| AsText(EndPoint(GeomFromText(@ls))) |
+-------------------------------------+
| POINT(3 3)                          |
+-------------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">104</field>
		<field name="name">COMPRESS</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
COMPRESS(string_to_compress)

Compresses a string and returns the result as a binary string. This
function requires MySQL to have been compiled with a compression
library such as zlib. Otherwise, the return value is always NULL. The
compressed string can be uncompressed with UNCOMPRESS().

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example">mysql&gt; SELECT LENGTH(COMPRESS(REPEAT('a',1000)));
        -&gt; 21
mysql&gt; SELECT LENGTH(COMPRESS(''));
        -&gt; 0
mysql&gt; SELECT LENGTH(COMPRESS('a'));
        -&gt; 13
mysql&gt; SELECT LENGTH(COMPRESS(REPEAT('a',16)));
        -&gt; 15
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">105</field>
		<field name="name">INSERT</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
INSERT [LOW_PRIORITY | DELAYED | HIGH_PRIORITY] [IGNORE]
    [INTO] tbl_name [(col_name,...)]
    {VALUES | VALUE} ({expr | DEFAULT},...),(...),...
    [ ON DUPLICATE KEY UPDATE
      col_name=expr
        [, col_name=expr] ... ]

Or:

INSERT [LOW_PRIORITY | DELAYED | HIGH_PRIORITY] [IGNORE]
    [INTO] tbl_name
    SET col_name={expr | DEFAULT}, ...
    [ ON DUPLICATE KEY UPDATE
      col_name=expr
        [, col_name=expr] ... ]

Or:

INSERT [LOW_PRIORITY | HIGH_PRIORITY] [IGNORE]
    [INTO] tbl_name [(col_name,...)]
    SELECT ...
    [ ON DUPLICATE KEY UPDATE
      col_name=expr
        [, col_name=expr] ... ]

INSERT inserts new rows into an existing table. The INSERT ... VALUES
and INSERT ... SET forms of the statement insert rows based on
explicitly specified values. The INSERT ... SELECT form inserts rows
selected from another table or tables. INSERT ... SELECT is discussed
further in [HELP INSERT SELECT].

URL: http://dev.mysql.com/doc/refman/5.5/en/insert.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/insert.html</field>
	</row>
	<row>
		<field name="help_topic_id">106</field>
		<field name="name">COUNT</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
COUNT(expr)

Returns a count of the number of non-NULL values of expr in the rows
retrieved by a SELECT statement. The result is a BIGINT value.

COUNT() returns 0 if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example">mysql&gt; SELECT student.student_name,COUNT(*)
    -&gt;        FROM student,course
    -&gt;        WHERE student.student_id=course.student_id
    -&gt;        GROUP BY student_name;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">107</field>
		<field name="name">HANDLER</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
HANDLER tbl_name OPEN [ [AS] alias]

HANDLER tbl_name READ index_name { = | &lt;= | &gt;= | &lt; | &gt; } (value1,value2,...)
    [ WHERE where_condition ] [LIMIT ... ]
HANDLER tbl_name READ index_name { FIRST | NEXT | PREV | LAST }
    [ WHERE where_condition ] [LIMIT ... ]
HANDLER tbl_name READ { FIRST | NEXT }
    [ WHERE where_condition ] [LIMIT ... ]

HANDLER tbl_name CLOSE

The HANDLER statement provides direct access to table storage engine
interfaces. It is available for InnoDB and MyISAM tables.

URL: http://dev.mysql.com/doc/refman/5.5/en/handler.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/handler.html</field>
	</row>
	<row>
		<field name="help_topic_id">108</field>
		<field name="name">MLINEFROMTEXT</field>
		<field name="help_category_id">3</field>
		<field name="description">MLineFromText(wkt[,srid]), MultiLineStringFromText(wkt[,srid])

Constructs a MULTILINESTRING value using its WKT representation and
SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">109</field>
		<field name="name">GEOMCOLLFROMWKB</field>
		<field name="help_category_id">33</field>
		<field name="description">GeomCollFromWKB(wkb[,srid]), GeometryCollectionFromWKB(wkb[,srid])

Constructs a GEOMETRYCOLLECTION value using its WKB representation and
SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">110</field>
		<field name="name">HELP_DATE</field>
		<field name="help_category_id">9</field>
		<field name="description">This help information was generated from the MySQL 5.5 Reference Manual
on: 2013-03-22
</field>
		<field name="example"></field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_topic_id">111</field>
		<field name="name">RENAME TABLE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
RENAME TABLE tbl_name TO new_tbl_name
    [, tbl_name2 TO new_tbl_name2] ...

This statement renames one or more tables.

The rename operation is done atomically, which means that no other
session can access any of the tables while the rename is running. For
example, if you have an existing table old_table, you can create
another table new_table that has the same structure but is empty, and
then replace the existing table with the empty one as follows (assuming
that backup_table does not already exist):

URL: http://dev.mysql.com/doc/refman/5.5/en/rename-table.html

</field>
		<field name="example">CREATE TABLE new_table (...);
RENAME TABLE old_table TO backup_table, new_table TO old_table;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/rename-table.html</field>
	</row>
	<row>
		<field name="help_topic_id">112</field>
		<field name="name">BOOLEAN</field>
		<field name="help_category_id">23</field>
		<field name="description">BOOL, BOOLEAN

These types are synonyms for TINYINT(1). A value of zero is considered
false. Nonzero values are considered true:

mysql&gt; SELECT IF(0, 'true', 'false');
+------------------------+
| IF(0, 'true', 'false') |
+------------------------+
| false                  |
+------------------------+

mysql&gt; SELECT IF(1, 'true', 'false');
+------------------------+
| IF(1, 'true', 'false') |
+------------------------+
| true                   |
+------------------------+

mysql&gt; SELECT IF(2, 'true', 'false');
+------------------------+
| IF(2, 'true', 'false') |
+------------------------+
| true                   |
+------------------------+

However, the values TRUE and FALSE are merely aliases for 1 and 0,
respectively, as shown here:

mysql&gt; SELECT IF(0 = FALSE, 'true', 'false');
+--------------------------------+
| IF(0 = FALSE, 'true', 'false') |
+--------------------------------+
| true                           |
+--------------------------------+

mysql&gt; SELECT IF(1 = TRUE, 'true', 'false');
+-------------------------------+
| IF(1 = TRUE, 'true', 'false') |
+-------------------------------+
| true                          |
+-------------------------------+

mysql&gt; SELECT IF(2 = TRUE, 'true', 'false');
+-------------------------------+
| IF(2 = TRUE, 'true', 'false') |
+-------------------------------+
| false                         |
+-------------------------------+

mysql&gt; SELECT IF(2 = FALSE, 'true', 'false');
+--------------------------------+
| IF(2 = FALSE, 'true', 'false') |
+--------------------------------+
| false                          |
+--------------------------------+

The last two statements display the results shown because 2 is equal to
neither 1 nor 0.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">113</field>
		<field name="name">DEFAULT</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
DEFAULT(col_name)

Returns the default value for a table column. An error results if the
column has no default value.

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example">mysql&gt; UPDATE t SET i = DEFAULT(i)+1 WHERE id &lt; 100;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">114</field>
		<field name="name">MOD</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
MOD(N,M), N % M, N MOD M

Modulo operation. Returns the remainder of N divided by M.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT MOD(234, 10);
        -&gt; 4
mysql&gt; SELECT 253 % 7;
        -&gt; 1
mysql&gt; SELECT MOD(29,9);
        -&gt; 2
mysql&gt; SELECT 29 MOD 9;
        -&gt; 2
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">115</field>
		<field name="name">TINYTEXT</field>
		<field name="help_category_id">23</field>
		<field name="description">TINYTEXT [CHARACTER SET charset_name] [COLLATE collation_name]

A TEXT column with a maximum length of 255 (28 - 1) characters. The
effective maximum length is less if the value contains multi-byte
characters. Each TINYTEXT value is stored using a 1-byte length prefix
that indicates the number of bytes in the value.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">116</field>
		<field name="name">OPTIMIZE TABLE</field>
		<field name="help_category_id">21</field>
		<field name="description">Syntax:
OPTIMIZE [NO_WRITE_TO_BINLOG | LOCAL] TABLE
    tbl_name [, tbl_name] ...

Reorganizes the physical storage of table data and associated index
data, to reduce storage space and improve I/O efficiency when accessing
the table. The exact changes made to each table depend on the storage
engine used by that table.

Use OPTIMIZE TABLE in these cases, depending on the type of table:

o After doing substantial insert, update, or delete operations on an
  InnoDB table that has its own .ibd file because it was created with
  the innodb_file_per_table option enabled. The table and indexes are
  reorganized, and disk space can be reclaimed for use by the operating
  system.

o After deleting a large part of a MyISAM or ARCHIVE table, or making
  many changes to a MyISAM or ARCHIVE table with variable-length rows
  (tables that have VARCHAR, VARBINARY, BLOB, or TEXT columns). Deleted
  rows are maintained in a linked list and subsequent INSERT operations
  reuse old row positions. You can use OPTIMIZE TABLE to reclaim the
  unused space and to defragment the data file. After extensive changes
  to a table, this statement may also improve performance of statements
  that use the table, sometimes significantly.

This statement requires SELECT and INSERT privileges for the table.

OPTIMIZE TABLE is also supported for partitioned tables. For
information about using this statement with partitioned tables and
table partitions, see
http://dev.mysql.com/doc/refman/5.5/en/partitioning-maintenance.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/optimize-table.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/optimize-table.html</field>
	</row>
	<row>
		<field name="help_topic_id">117</field>
		<field name="name">DECODE</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
DECODE(crypt_str,pass_str)

Decrypts the encrypted string crypt_str using pass_str as the password.
crypt_str should be a string returned from ENCODE().

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">118</field>
		<field name="name">&lt;=&gt;</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
&lt;=&gt;

NULL-safe equal. This operator performs an equality comparison like the
= operator, but returns 1 rather than NULL if both operands are NULL,
and 0 rather than NULL if one operand is NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT 1 &lt;=&gt; 1, NULL &lt;=&gt; NULL, 1 &lt;=&gt; NULL;
        -&gt; 1, 1, 0
mysql&gt; SELECT 1 = 1, NULL = NULL, 1 = NULL;
        -&gt; 1, NULL, NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">119</field>
		<field name="name">HELP STATEMENT</field>
		<field name="help_category_id">29</field>
		<field name="description">Syntax:
HELP 'search_string'

The HELP statement returns online information from the MySQL Reference
manual. Its proper operation requires that the help tables in the mysql
database be initialized with help topic information (see
http://dev.mysql.com/doc/refman/5.5/en/server-side-help-support.html).

The HELP statement searches the help tables for the given search string
and displays the result of the search. The search string is not case
sensitive.

URL: http://dev.mysql.com/doc/refman/5.5/en/help.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/help.html</field>
	</row>
	<row>
		<field name="help_topic_id">120</field>
		<field name="name">RESET</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
RESET reset_option [, reset_option] ...

The RESET statement is used to clear the state of various server
operations. You must have the RELOAD privilege to execute RESET.

RESET acts as a stronger version of the FLUSH statement. See [HELP
FLUSH].

The RESET statement causes an implicit commit. See
http://dev.mysql.com/doc/refman/5.5/en/implicit-commit.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/reset.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/reset.html</field>
	</row>
	<row>
		<field name="help_topic_id">121</field>
		<field name="name">GET_LOCK</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
GET_LOCK(str,timeout)

Tries to obtain a lock with a name given by the string str, using a
timeout of timeout seconds. Returns 1 if the lock was obtained
successfully, 0 if the attempt timed out (for example, because another
client has previously locked the name), or NULL if an error occurred
(such as running out of memory or the thread was killed with mysqladmin
kill). If you have a lock obtained with GET_LOCK(), it is released when
you execute RELEASE_LOCK(), execute a new GET_LOCK(), or your
connection terminates (either normally or abnormally). Locks obtained
with GET_LOCK() do not interact with transactions. That is, committing
a transaction does not release any such locks obtained during the
transaction.

This function can be used to implement application locks or to simulate
record locks. Names are locked on a server-wide basis. If a name has
been locked by one client, GET_LOCK() blocks any request by another
client for a lock with the same name. This enables clients that agree
on a given lock name to use the name to perform cooperative advisory
locking. But be aware that it also enables a client that is not among
the set of cooperating clients to lock a name, either inadvertently or
deliberately, and thus prevent any of the cooperating clients from
locking that name. One way to reduce the likelihood of this is to use
lock names that are database-specific or application-specific. For
example, use lock names of the form db_name.str or app_name.str.

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example">mysql&gt; SELECT GET_LOCK('lock1',10);
        -&gt; 1
mysql&gt; SELECT IS_FREE_LOCK('lock2');
        -&gt; 1
mysql&gt; SELECT GET_LOCK('lock2',10);
        -&gt; 1
mysql&gt; SELECT RELEASE_LOCK('lock2');
        -&gt; 1
mysql&gt; SELECT RELEASE_LOCK('lock1');
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">122</field>
		<field name="name">UCASE</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
UCASE(str)

UCASE() is a synonym for UPPER().

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">123</field>
		<field name="name">SHOW BINLOG EVENTS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW BINLOG EVENTS
   [IN 'log_name'] [FROM pos] [LIMIT [offset,] row_count]

Shows the events in the binary log. If you do not specify 'log_name',
the first binary log is displayed.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-binlog-events.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-binlog-events.html</field>
	</row>
	<row>
		<field name="help_topic_id">124</field>
		<field name="name">MPOLYFROMWKB</field>
		<field name="help_category_id">33</field>
		<field name="description">MPolyFromWKB(wkb[,srid]), MultiPolygonFromWKB(wkb[,srid])

Constructs a MULTIPOLYGON value using its WKB representation and SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">125</field>
		<field name="name">ITERATE</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
ITERATE label

ITERATE can appear only within LOOP, REPEAT, and WHILE statements.
ITERATE means &quot;start the loop again.&quot;

URL: http://dev.mysql.com/doc/refman/5.5/en/iterate.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/iterate.html</field>
	</row>
	<row>
		<field name="help_topic_id">126</field>
		<field name="name">DO</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
DO expr [, expr] ...

DO executes the expressions but does not return any results. In most
respects, DO is shorthand for SELECT expr, ..., but has the advantage
that it is slightly faster when you do not care about the result.

DO is useful primarily with functions that have side effects, such as
RELEASE_LOCK().

URL: http://dev.mysql.com/doc/refman/5.5/en/do.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/do.html</field>
	</row>
	<row>
		<field name="help_topic_id">127</field>
		<field name="name">CURTIME</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
CURTIME()

Returns the current time as a value in 'HH:MM:SS' or HHMMSS.uuuuuu
format, depending on whether the function is used in a string or
numeric context. The value is expressed in the current time zone.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT CURTIME();
        -&gt; '23:50:26'
mysql&gt; SELECT CURTIME() + 0;
        -&gt; 235026.000000
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">128</field>
		<field name="name">CHAR_LENGTH</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
CHAR_LENGTH(str)

Returns the length of the string str, measured in characters. A
multi-byte character counts as a single character. This means that for
a string containing five 2-byte characters, LENGTH() returns 10,
whereas CHAR_LENGTH() returns 5.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">129</field>
		<field name="name">BIGINT</field>
		<field name="help_category_id">23</field>
		<field name="description">BIGINT[(M)] [UNSIGNED] [ZEROFILL]

A large integer. The signed range is -9223372036854775808 to
9223372036854775807. The unsigned range is 0 to 18446744073709551615.

SERIAL is an alias for BIGINT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">130</field>
		<field name="name">SET</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SET variable_assignment [, variable_assignment] ...

variable_assignment:
      user_var_name = expr
    | [GLOBAL | SESSION] system_var_name = expr
    | [@@global. | @@session. | @@]system_var_name = expr

The SET statement assigns values to different types of variables that
affect the operation of the server or your client. Older versions of
MySQL employed SET OPTION, but this syntax is deprecated in favor of
SET without OPTION.

URL: http://dev.mysql.com/doc/refman/5.5/en/set-statement.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/set-statement.html</field>
	</row>
	<row>
		<field name="help_topic_id">131</field>
		<field name="name">LOAD XML</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
LOAD XML [LOW_PRIORITY | CONCURRENT] [LOCAL] INFILE 'file_name'
    [REPLACE | IGNORE]
    INTO TABLE [db_name.]tbl_name
    [CHARACTER SET charset_name]
    [ROWS IDENTIFIED BY '&lt;tagname&gt;']
    [IGNORE number {LINES | ROWS}]
    [(column_or_user_var,...)]
    [SET col_name = expr,...]

The LOAD XML statement reads data from an XML file into a table. The
file_name must be given as a literal string. The tagname in the
optional ROWS IDENTIFIED BY clause must also be given as a literal
string, and must be surrounded by angle brackets (&lt; and &gt;).

LOAD XML acts as the complement of running the mysql client in XML
output mode (that is, starting the client with the --xml option). To
write data from a table to an XML file, use a command such as the
following one from the system shell:

shell&gt; mysql --xml -e 'SELECT * FROM mytable' &gt; file.xml

To read the file back into a table, use LOAD XML INFILE. By default,
the &lt;row&gt; element is considered to be the equivalent of a database
table row; this can be changed using the ROWS IDENTIFIED BY clause.

This statement supports three different XML formats:

o Column names as attributes and column values as attribute values:

&lt;row column1=&quot;value1&quot; column2=&quot;value2&quot; .../&gt;

o Column names as tags and column values as the content of these tags:

&lt;row&gt;
  &lt;column1&gt;value1&lt;/column1&gt;
  &lt;column2&gt;value2&lt;/column2&gt;
&lt;/row&gt;

o Column names are the name attributes of &lt;field&gt; tags, and values are
  the contents of these tags:

&lt;row&gt;
  &lt;field name='column1'&gt;value1&lt;/field&gt;
  &lt;field name='column2'&gt;value2&lt;/field&gt;
&lt;/row&gt;

  This is the format used by other MySQL tools, such as mysqldump.

All 3 formats can be used in the same XML file; the import routine
automatically detects the format for each row and interprets it
correctly. Tags are matched based on the tag or attribute name and the
column name.

The following clauses work essentially the same way for LOAD XML as
they do for LOAD DATA:

o LOW_PRIORITY or CONCURRENT

o LOCAL

o REPLACE or IGNORE

o CHARACTER SET

o (column_or_user_var,...)

o SET

See [HELP LOAD DATA], for more information about these clauses.

The IGNORE number LINES or IGNORE number ROWS clause causes the first
number rows in the XML file to be skipped. It is analogous to the LOAD
DATA statement's IGNORE ... LINES clause.

URL: http://dev.mysql.com/doc/refman/5.5/en/load-xml.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/load-xml.html</field>
	</row>
	<row>
		<field name="help_topic_id">132</field>
		<field name="name">CONV</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
CONV(N,from_base,to_base)

Converts numbers between different number bases. Returns a string
representation of the number N, converted from base from_base to base
to_base. Returns NULL if any argument is NULL. The argument N is
interpreted as an integer, but may be specified as an integer or a
string. The minimum base is 2 and the maximum base is 36. If to_base is
a negative number, N is regarded as a signed number. Otherwise, N is
treated as unsigned. CONV() works with 64-bit precision.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT CONV('a',16,2);
        -&gt; '1010'
mysql&gt; SELECT CONV('6E',18,8);
        -&gt; '172'
mysql&gt; SELECT CONV(-17,10,-18);
        -&gt; '-H'
mysql&gt; SELECT CONV(10+'10'+'10'+0xa,10,10);
        -&gt; '40'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">133</field>
		<field name="name">DATE</field>
		<field name="help_category_id">23</field>
		<field name="description">DATE

A date. The supported range is '1000-01-01' to '9999-12-31'. MySQL
displays DATE values in 'YYYY-MM-DD' format, but permits assignment of
values to DATE columns using either strings or numbers.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">134</field>
		<field name="name">ASSIGN-VALUE</field>
		<field name="help_category_id">15</field>
		<field name="description">Syntax:
:=

Assignment operator. Causes the user variable on the left hand side of
the operator to take on the value to its right. The value on the right
hand side may be a literal value, another variable storing a value, or
any legal expression that yields a scalar value, including the result
of a query (provided that this value is a scalar value). You can
perform multiple assignments in the same SET statement. You can perform
multiple assignments in the same statement-

Unlike =, the := operator is never interpreted as a comparison
operator. This means you can use := in any valid SQL statement (not
just in SET statements) to assign a value to a variable.

URL: http://dev.mysql.com/doc/refman/5.5/en/assignment-operators.html

</field>
		<field name="example">mysql&gt; SELECT @var1, @var2;
        -&gt; NULL, NULL
mysql&gt; SELECT @var1 := 1, @var2;
        -&gt; 1, NULL
mysql&gt; SELECT @var1, @var2;
        -&gt; 1, NULL
mysql&gt; SELECT @var1, @var2 := @var1;
        -&gt; 1, 1
mysql&gt; SELECT @var1, @var2;
        -&gt; 1, 1

mysql&gt; SELECT @var1:=COUNT(*) FROM t1;
        -&gt; 4
mysql&gt; SELECT @var1;
        -&gt; 4
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/assignment-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">135</field>
		<field name="name">SHOW OPEN TABLES</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW OPEN TABLES [{FROM | IN} db_name]
    [LIKE 'pattern' | WHERE expr]

SHOW OPEN TABLES lists the non-TEMPORARY tables that are currently open
in the table cache. See
http://dev.mysql.com/doc/refman/5.5/en/table-cache.html. The FROM
clause, if present, restricts the tables shown to those present in the
db_name database. The LIKE clause, if present, indicates which table
names to match. The WHERE clause can be given to select rows using more
general conditions, as discussed in
http://dev.mysql.com/doc/refman/5.5/en/extended-show.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-open-tables.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-open-tables.html</field>
	</row>
	<row>
		<field name="help_topic_id">136</field>
		<field name="name">EXTRACT</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
EXTRACT(unit FROM date)

The EXTRACT() function uses the same kinds of unit specifiers as
DATE_ADD() or DATE_SUB(), but extracts parts from the date rather than
performing date arithmetic.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT EXTRACT(YEAR FROM '2009-07-02');
       -&gt; 2009
mysql&gt; SELECT EXTRACT(YEAR_MONTH FROM '2009-07-02 01:02:03');
       -&gt; 200907
mysql&gt; SELECT EXTRACT(DAY_MINUTE FROM '2009-07-02 01:02:03');
       -&gt; 20102
mysql&gt; SELECT EXTRACT(MICROSECOND
    -&gt;                FROM '2003-01-02 10:30:00.000123');
        -&gt; 123
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">137</field>
		<field name="name">ENCRYPT</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
ENCRYPT(str[,salt])

Encrypts str using the Unix crypt() system call and returns a binary
string. The salt argument must be a string with at least two characters
or the result will be NULL. If no salt argument is given, a random
value is used.

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example">mysql&gt; SELECT ENCRYPT('hello');
        -&gt; 'VxuFAJXVARROc'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">138</field>
		<field name="name">SHOW STATUS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW [GLOBAL | SESSION] STATUS
    [LIKE 'pattern' | WHERE expr]

SHOW STATUS provides server status information. This information also
can be obtained using the mysqladmin extended-status command. The LIKE
clause, if present, indicates which variable names to match. The WHERE
clause can be given to select rows using more general conditions, as
discussed in http://dev.mysql.com/doc/refman/5.5/en/extended-show.html.
This statement does not require any privilege. It requires only the
ability to connect to the server.
With a LIKE clause, the statement displays only rows for those
variables with names that match the pattern:

mysql&gt; SHOW STATUS LIKE 'Key%';
+--------------------+----------+
| Variable_name      | Value    |
+--------------------+----------+
| Key_blocks_used    | 14955    |
| Key_read_requests  | 96854827 |
| Key_reads          | 162040   |
| Key_write_requests | 7589728  |
| Key_writes         | 3813196  |
+--------------------+----------+

With the GLOBAL modifier, SHOW STATUS displays the status values for
all connections to MySQL. With SESSION, it displays the status values
for the current connection. If no modifier is present, the default is
SESSION. LOCAL is a synonym for SESSION.

Some status variables have only a global value. For these, you get the
same value for both GLOBAL and SESSION. The scope for each status
variable is listed at
http://dev.mysql.com/doc/refman/5.5/en/server-status-variables.html.

Each invocation of the SHOW STATUS statement uses an internal temporary
table and increments the global Created_tmp_tables value.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-status.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-status.html</field>
	</row>
	<row>
		<field name="help_topic_id">139</field>
		<field name="name">EXTRACTVALUE</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
ExtractValue(xml_frag, xpath_expr)

ExtractValue() takes two string arguments, a fragment of XML markup
xml_frag and an XPath expression xpath_expr (also known as a locator);
it returns the text (CDATA) of the first text node which is a child of
the elements or elements matched by the XPath expression. In MySQL 5.5,
the XPath expression can contain at most 127 characters. (This
limitation is lifted in MySQL 5.6.)

Using this function is the equivalent of performing a match using the
xpath_expr after appending /text(). In other words,
ExtractValue('&lt;a&gt;&lt;b&gt;Sakila&lt;/b&gt;&lt;/a&gt;', '/a/b') and
ExtractValue('&lt;a&gt;&lt;b&gt;Sakila&lt;/b&gt;&lt;/a&gt;', '/a/b/text()') produce the same
result.

If multiple matches are found, the content of the first child text node
of each matching element is returned (in the order matched) as a
single, space-delimited string.

If no matching text node is found for the expression (including the
implicit /text())---for whatever reason, as long as xpath_expr is
valid, and xml_frag consists of elements which are properly nested and
closed---an empty string is returned. No distinction is made between a
match on an empty element and no match at all. This is by design.

If you need to determine whether no matching element was found in
xml_frag or such an element was found but contained no child text
nodes, you should test the result of an expression that uses the XPath
count() function. For example, both of these statements return an empty
string, as shown here:

mysql&gt; SELECT ExtractValue('&lt;a&gt;&lt;b/&gt;&lt;/a&gt;', '/a/b');
+-------------------------------------+
| ExtractValue('&lt;a&gt;&lt;b/&gt;&lt;/a&gt;', '/a/b') |
+-------------------------------------+
|                                     |
+-------------------------------------+
1 row in set (0.00 sec)

mysql&gt; SELECT ExtractValue('&lt;a&gt;&lt;c/&gt;&lt;/a&gt;', '/a/b');
+-------------------------------------+
| ExtractValue('&lt;a&gt;&lt;c/&gt;&lt;/a&gt;', '/a/b') |
+-------------------------------------+
|                                     |
+-------------------------------------+
1 row in set (0.00 sec)

However, you can determine whether there was actually a matching
element using the following:

mysql&gt; SELECT ExtractValue('&lt;a&gt;&lt;b/&gt;&lt;/a&gt;', 'count(/a/b)');
+-------------------------------------+
| ExtractValue('&lt;a&gt;&lt;b/&gt;&lt;/a&gt;', 'count(/a/b)') |
+-------------------------------------+
| 1                                   |
+-------------------------------------+
1 row in set (0.00 sec)

mysql&gt; SELECT ExtractValue('&lt;a&gt;&lt;c/&gt;&lt;/a&gt;', 'count(/a/b)');
+-------------------------------------+
| ExtractValue('&lt;a&gt;&lt;c/&gt;&lt;/a&gt;', 'count(/a/b)') |
+-------------------------------------+
| 0                                   |
+-------------------------------------+
1 row in set (0.01 sec)

*Important*: ExtractValue() returns only CDATA, and does not return any
tags that might be contained within a matching tag, nor any of their
content (see the result returned as val1 in the following example).

URL: http://dev.mysql.com/doc/refman/5.5/en/xml-functions.html

</field>
		<field name="example">mysql&gt; SELECT
    -&gt;   ExtractValue('&lt;a&gt;ccc&lt;b&gt;ddd&lt;/b&gt;&lt;/a&gt;', '/a') AS val1,
    -&gt;   ExtractValue('&lt;a&gt;ccc&lt;b&gt;ddd&lt;/b&gt;&lt;/a&gt;', '/a/b') AS val2,
    -&gt;   ExtractValue('&lt;a&gt;ccc&lt;b&gt;ddd&lt;/b&gt;&lt;/a&gt;', '//b') AS val3,
    -&gt;   ExtractValue('&lt;a&gt;ccc&lt;b&gt;ddd&lt;/b&gt;&lt;/a&gt;', '/b') AS val4,
    -&gt;   ExtractValue('&lt;a&gt;ccc&lt;b&gt;ddd&lt;/b&gt;&lt;b&gt;eee&lt;/b&gt;&lt;/a&gt;', '//b') AS val5;

+------+------+------+------+---------+
| val1 | val2 | val3 | val4 | val5    |
+------+------+------+------+---------+
| ccc  | ddd  | ddd  |      | ddd eee |
+------+------+------+------+---------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/xml-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">140</field>
		<field name="name">OLD_PASSWORD</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
OLD_PASSWORD(str)

OLD_PASSWORD() was added when the implementation of PASSWORD() was
changed in MySQL 4.1 to improve security. OLD_PASSWORD() returns the
value of the pre-4.1 implementation of PASSWORD() as a string, and is
intended to permit you to reset passwords for any pre-4.1 clients that
need to connect to your version 5.5 MySQL server without locking them
out. See http://dev.mysql.com/doc/refman/5.5/en/password-hashing.html.

As of MySQL 5.5.3, the return value is a nonbinary string in the
connection character set. Before 5.5.3, the return value is a binary
string.

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">141</field>
		<field name="name">FORMAT</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
FORMAT(X,D[,locale])

Formats the number X to a format like '#,###,###.##', rounded to D
decimal places, and returns the result as a string. If D is 0, the
result has no decimal point or fractional part.

The optional third parameter enables a locale to be specified to be
used for the result number's decimal point, thousands separator, and
grouping between separators. Permissible locale values are the same as
the legal values for the lc_time_names system variable (see
http://dev.mysql.com/doc/refman/5.5/en/locale-support.html). If no
locale is specified, the default is 'en_US'.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT FORMAT(12332.123456, 4);
        -&gt; '12,332.1235'
mysql&gt; SELECT FORMAT(12332.1,4);
        -&gt; '12,332.1000'
mysql&gt; SELECT FORMAT(12332.2,0);
        -&gt; '12,332'
mysql&gt; SELECT FORMAT(12332.2,2,'de_DE');
        -&gt; '12.332,20'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">142</field>
		<field name="name">||</field>
		<field name="help_category_id">15</field>
		<field name="description">Syntax:
OR, ||

Logical OR. When both operands are non-NULL, the result is 1 if any
operand is nonzero, and 0 otherwise. With a NULL operand, the result is
1 if the other operand is nonzero, and NULL otherwise. If both operands
are NULL, the result is NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/logical-operators.html

</field>
		<field name="example">mysql&gt; SELECT 1 || 1;
        -&gt; 1
mysql&gt; SELECT 1 || 0;
        -&gt; 1
mysql&gt; SELECT 0 || 0;
        -&gt; 0
mysql&gt; SELECT 0 || NULL;
        -&gt; NULL
mysql&gt; SELECT 1 || NULL;
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/logical-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">143</field>
		<field name="name">BIT_LENGTH</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
BIT_LENGTH(str)

Returns the length of the string str in bits.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT BIT_LENGTH('text');
        -&gt; 32
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">144</field>
		<field name="name">EXTERIORRING</field>
		<field name="help_category_id">2</field>
		<field name="description">ExteriorRing(poly)

Returns the exterior ring of the Polygon value poly as a LineString.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SET @poly =
    -&gt; 'Polygon((0 0,0 3,3 3,3 0,0 0),(1 1,1 2,2 2,2 1,1 1))';
mysql&gt; SELECT AsText(ExteriorRing(GeomFromText(@poly)));
+-------------------------------------------+
| AsText(ExteriorRing(GeomFromText(@poly))) |
+-------------------------------------------+
| LINESTRING(0 0,0 3,3 3,3 0,0 0)           |
+-------------------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">145</field>
		<field name="name">GEOMFROMWKB</field>
		<field name="help_category_id">33</field>
		<field name="description">GeomFromWKB(wkb[,srid]), GeometryFromWKB(wkb[,srid])

Constructs a geometry value of any type using its WKB representation
and SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">146</field>
		<field name="name">SHOW SLAVE HOSTS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW SLAVE HOSTS

Displays a list of replication slaves currently registered with the
master. (Before MySQL 5.5.3, only slaves started with the
--report-host=host_name option are visible in this list.)

The list is displayed on any server (not just the master server). The
output looks like this:

mysql&gt; SHOW SLAVE HOSTS;
+------------+-----------+------+-----------+
| Server_id  | Host      | Port | Master_id |
+------------+-----------+------+-----------+
|  192168010 | iconnect2 | 3306 | 192168011 |
| 1921680101 | athena    | 3306 | 192168011 |
+------------+-----------+------+-----------+

o Server_id: The unique server ID of the slave server, as configured in
  the server's option file, or on the command line with
  --server-id=value.

o Host: The host name of the slave server, as configured in the
  server's option file, or on the command line with
  --report-host=host_name. Note that this can differ from the machine
  name as configured in the operating system.

o Port: The port the slave server is listening on.

  In MySQL 5.5.23 and later, a zero in this column means that the slave
  port (--report-port) was not set. Prior to MySQL 5.5.23, 3306 was
  used as the default in such cases (Bug #13333431).

o Master_id: The unique server ID of the master server that the slave
  server is replicating from.

Some MySQL versions report another variable, Rpl_recovery_rank. This
variable was never used, and was removed in MySQL 5.5.3. (Bug #13963)

URL: http://dev.mysql.com/doc/refman/5.5/en/show-slave-hosts.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-slave-hosts.html</field>
	</row>
	<row>
		<field name="help_topic_id">147</field>
		<field name="name">START TRANSACTION</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
START TRANSACTION [WITH CONSISTENT SNAPSHOT]
BEGIN [WORK]
COMMIT [WORK] [AND [NO] CHAIN] [[NO] RELEASE]
ROLLBACK [WORK] [AND [NO] CHAIN] [[NO] RELEASE]
SET autocommit = {0 | 1}

These statements provide control over use of transactions:

o START TRANSACTION or BEGIN start a new transaction.

o COMMIT commits the current transaction, making its changes permanent.

o ROLLBACK rolls back the current transaction, canceling its changes.

o SET autocommit disables or enables the default autocommit mode for
  the current session.

By default, MySQL runs with autocommit mode enabled. This means that as
soon as you execute a statement that updates (modifies) a table, MySQL
stores the update on disk to make it permanent. The change cannot be
rolled back.

To disable autocommit mode implicitly for a single series of
statements, use the START TRANSACTION statement:

START TRANSACTION;
SELECT @A:=SUM(salary) FROM table1 WHERE type=1;
UPDATE table2 SET summary=@A WHERE type=1;
COMMIT;

With START TRANSACTION, autocommit remains disabled until you end the
transaction with COMMIT or ROLLBACK. The autocommit mode then reverts
to its previous state.

You can also begin a transaction like this:

START TRANSACTION WITH CONSISTENT SNAPSHOT;

The WITH CONSISTENT SNAPSHOT option starts a consistent read for
storage engines that are capable of it. This applies only to InnoDB.
The effect is the same as issuing a START TRANSACTION followed by a
SELECT from any InnoDB table. See
http://dev.mysql.com/doc/refman/5.5/en/innodb-consistent-read.html. The
WITH CONSISTENT SNAPSHOT option does not change the current transaction
isolation level, so it provides a consistent snapshot only if the
current isolation level is one that permits consistent read (REPEATABLE
READ or SERIALIZABLE).

*Important*: Many APIs used for writing MySQL client applications (such
as JDBC) provide their own methods for starting transactions that can
(and sometimes should) be used instead of sending a START TRANSACTION
statement from the client. See
http://dev.mysql.com/doc/refman/5.5/en/connectors-apis.html, or the
documentation for your API, for more information.

To disable autocommit mode explicitly, use the following statement:

SET autocommit=0;

After disabling autocommit mode by setting the autocommit variable to
zero, changes to transaction-safe tables (such as those for InnoDB or
NDBCLUSTER) are not made permanent immediately. You must use COMMIT to
store your changes to disk or ROLLBACK to ignore the changes.

autocommit is a session variable and must be set for each session. To
disable autocommit mode for each new connection, see the description of
the autocommit system variable at
http://dev.mysql.com/doc/refman/5.5/en/server-system-variables.html.

BEGIN and BEGIN WORK are supported as aliases of START TRANSACTION for
initiating a transaction. START TRANSACTION is standard SQL syntax and
is the recommended way to start an ad-hoc transaction.

The BEGIN statement differs from the use of the BEGIN keyword that
starts a BEGIN ... END compound statement. The latter does not begin a
transaction. See [HELP BEGIN END].

*Note*: Within all stored programs (stored procedures and functions,
triggers, and events), the parser treats BEGIN [WORK] as the beginning
of a BEGIN ... END block. Begin a transaction in this context with
START TRANSACTION instead.

The optional WORK keyword is supported for COMMIT and ROLLBACK, as are
the CHAIN and RELEASE clauses. CHAIN and RELEASE can be used for
additional control over transaction completion. The value of the
completion_type system variable determines the default completion
behavior. See
http://dev.mysql.com/doc/refman/5.5/en/server-system-variables.html.

The AND CHAIN clause causes a new transaction to begin as soon as the
current one ends, and the new transaction has the same isolation level
as the just-terminated transaction. The RELEASE clause causes the
server to disconnect the current client session after terminating the
current transaction. Including the NO keyword suppresses CHAIN or
RELEASE completion, which can be useful if the completion_type system
variable is set to cause chaining or release completion by default.

URL: http://dev.mysql.com/doc/refman/5.5/en/commit.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/commit.html</field>
	</row>
	<row>
		<field name="help_topic_id">148</field>
		<field name="name">BETWEEN AND</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
expr BETWEEN min AND max

If expr is greater than or equal to min and expr is less than or equal
to max, BETWEEN returns 1, otherwise it returns 0. This is equivalent
to the expression (min &lt;= expr AND expr &lt;= max) if all the arguments
are of the same type. Otherwise type conversion takes place according
to the rules described in
http://dev.mysql.com/doc/refman/5.5/en/type-conversion.html, but
applied to all the three arguments.

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT 2 BETWEEN 1 AND 3, 2 BETWEEN 3 and 1;
        -&gt; 1, 0
mysql&gt; SELECT 1 BETWEEN 2 AND 3;
        -&gt; 0
mysql&gt; SELECT 'b' BETWEEN 'a' AND 'c';
        -&gt; 1
mysql&gt; SELECT 2 BETWEEN 2 AND '3';
        -&gt; 1
mysql&gt; SELECT 2 BETWEEN 2 AND 'x-3';
        -&gt; 0
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">149</field>
		<field name="name">MULTIPOLYGON</field>
		<field name="help_category_id">25</field>
		<field name="description">MultiPolygon(poly1,poly2,...)

Constructs a MultiPolygon value from a set of Polygon or WKB Polygon
arguments.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">150</field>
		<field name="name">TIME_FORMAT</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
TIME_FORMAT(time,format)

This is used like the DATE_FORMAT() function, but the format string may
contain format specifiers only for hours, minutes, seconds, and
microseconds. Other specifiers produce a NULL value or 0.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT TIME_FORMAT('100:00:00', '%H %k %h %I %l');
        -&gt; '100 100 04 04 4'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">151</field>
		<field name="name">LEFT</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
LEFT(str,len)

Returns the leftmost len characters from the string str, or NULL if any
argument is NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT LEFT('foobarbar', 5);
        -&gt; 'fooba'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">152</field>
		<field name="name">FLUSH QUERY CACHE</field>
		<field name="help_category_id">27</field>
		<field name="description">You can defragment the query cache to better utilize its memory with
the FLUSH QUERY CACHE statement. The statement does not remove any
queries from the cache.

The RESET QUERY CACHE statement removes all query results from the
query cache. The FLUSH TABLES statement also does this.

URL: http://dev.mysql.com/doc/refman/5.5/en/query-cache-status-and-maintenance.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/query-cache-status-and-maintenance.html</field>
	</row>
	<row>
		<field name="help_topic_id">153</field>
		<field name="name">SET DATA TYPE</field>
		<field name="help_category_id">23</field>
		<field name="description">SET('value1','value2',...) [CHARACTER SET charset_name] [COLLATE
collation_name]

A set. A string object that can have zero or more values, each of which
must be chosen from the list of values 'value1', 'value2', ... SET
values are represented internally as integers.

A SET column can have a maximum of 64 distinct members. A table can
have no more than 255 unique element list definitions among its ENUM
and SET columns considered as a group. For more information on this
limit, see http://dev.mysql.com/doc/refman/5.5/en/limits-frm-file.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">154</field>
		<field name="name">RAND</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
RAND(), RAND(N)

Returns a random floating-point value v in the range 0 &lt;= v &lt; 1.0. If a
constant integer argument N is specified, it is used as the seed value,
which produces a repeatable sequence of column values. In the following
example, note that the sequences of values produced by RAND(3) is the
same both places where it occurs.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; CREATE TABLE t (i INT);
Query OK, 0 rows affected (0.42 sec)

mysql&gt; INSERT INTO t VALUES(1),(2),(3);
Query OK, 3 rows affected (0.00 sec)
Records: 3  Duplicates: 0  Warnings: 0

mysql&gt; SELECT i, RAND() FROM t;
+------+------------------+
| i    | RAND()           |
+------+------------------+
|    1 | 0.61914388706828 |
|    2 | 0.93845168309142 |
|    3 | 0.83482678498591 |
+------+------------------+
3 rows in set (0.00 sec)

mysql&gt; SELECT i, RAND(3) FROM t;
+------+------------------+
| i    | RAND(3)          |
+------+------------------+
|    1 | 0.90576975597606 |
|    2 | 0.37307905813035 |
|    3 | 0.14808605345719 |
+------+------------------+
3 rows in set (0.00 sec)

mysql&gt; SELECT i, RAND() FROM t;
+------+------------------+
| i    | RAND()           |
+------+------------------+
|    1 | 0.35877890638893 |
|    2 | 0.28941420772058 |
|    3 | 0.37073435016976 |
+------+------------------+
3 rows in set (0.00 sec)

mysql&gt; SELECT i, RAND(3) FROM t;
+------+------------------+
| i    | RAND(3)          |
+------+------------------+
|    1 | 0.90576975597606 |
|    2 | 0.37307905813035 |
|    3 | 0.14808605345719 |
+------+------------------+
3 rows in set (0.01 sec)
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">155</field>
		<field name="name">RPAD</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
RPAD(str,len,padstr)

Returns the string str, right-padded with the string padstr to a length
of len characters. If str is longer than len, the return value is
shortened to len characters.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT RPAD('hi',5,'?');
        -&gt; 'hi???'
mysql&gt; SELECT RPAD('hi',1,'?');
        -&gt; 'h'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">156</field>
		<field name="name">CREATE DATABASE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
CREATE {DATABASE | SCHEMA} [IF NOT EXISTS] db_name
    [create_specification] ...

create_specification:
    [DEFAULT] CHARACTER SET [=] charset_name
  | [DEFAULT] COLLATE [=] collation_name

CREATE DATABASE creates a database with the given name. To use this
statement, you need the CREATE privilege for the database. CREATE
SCHEMA is a synonym for CREATE DATABASE.

URL: http://dev.mysql.com/doc/refman/5.5/en/create-database.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-database.html</field>
	</row>
	<row>
		<field name="help_topic_id">157</field>
		<field name="name">DEC</field>
		<field name="help_category_id">23</field>
		<field name="description">DEC[(M[,D])] [UNSIGNED] [ZEROFILL], NUMERIC[(M[,D])] [UNSIGNED]
[ZEROFILL], FIXED[(M[,D])] [UNSIGNED] [ZEROFILL]

These types are synonyms for DECIMAL. The FIXED synonym is available
for compatibility with other database systems.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">158</field>
		<field name="name">VAR_POP</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
VAR_POP(expr)

Returns the population standard variance of expr. It considers rows as
the whole population, not as a sample, so it has the number of rows as
the denominator. You can also use VARIANCE(), which is equivalent but
is not standard SQL.

VAR_POP() returns NULL if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">159</field>
		<field name="name">ELT</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
ELT(N,str1,str2,str3,...)

ELT() returns the Nth element of the list of strings: str1 if N = 1,
str2 if N = 2, and so on. Returns NULL if N is less than 1 or greater
than the number of arguments. ELT() is the complement of FIELD().

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT ELT(1, 'ej', 'Heja', 'hej', 'foo');
        -&gt; 'ej'
mysql&gt; SELECT ELT(4, 'ej', 'Heja', 'hej', 'foo');
        -&gt; 'foo'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">160</field>
		<field name="name">ALTER VIEW</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
ALTER
    [ALGORITHM = {UNDEFINED | MERGE | TEMPTABLE}]
    [DEFINER = { user | CURRENT_USER }]
    [SQL SECURITY { DEFINER | INVOKER }]
    VIEW view_name [(column_list)]
    AS select_statement
    [WITH [CASCADED | LOCAL] CHECK OPTION]

This statement changes the definition of a view, which must exist. The
syntax is similar to that for CREATE VIEW and the effect is the same as
for CREATE OR REPLACE VIEW. See [HELP CREATE VIEW]. This statement
requires the CREATE VIEW and DROP privileges for the view, and some
privilege for each column referred to in the SELECT statement. ALTER
VIEW is permitted only to the definer or users with the SUPER
privilege.

URL: http://dev.mysql.com/doc/refman/5.5/en/alter-view.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/alter-view.html</field>
	</row>
	<row>
		<field name="help_topic_id">161</field>
		<field name="name">SHOW DATABASES</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW {DATABASES | SCHEMAS}
    [LIKE 'pattern' | WHERE expr]

SHOW DATABASES lists the databases on the MySQL server host. SHOW
SCHEMAS is a synonym for SHOW DATABASES. The LIKE clause, if present,
indicates which database names to match. The WHERE clause can be given
to select rows using more general conditions, as discussed in
http://dev.mysql.com/doc/refman/5.5/en/extended-show.html.

You see only those databases for which you have some kind of privilege,
unless you have the global SHOW DATABASES privilege. You can also get
this list using the mysqlshow command.

If the server was started with the --skip-show-database option, you
cannot use this statement at all unless you have the SHOW DATABASES
privilege.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-databases.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-databases.html</field>
	</row>
	<row>
		<field name="help_topic_id">162</field>
		<field name="name">~</field>
		<field name="help_category_id">20</field>
		<field name="description">Syntax:
~

Invert all bits.

URL: http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html

</field>
		<field name="example">mysql&gt; SELECT 5 &amp; ~1;
        -&gt; 4
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">163</field>
		<field name="name">TEXT</field>
		<field name="help_category_id">23</field>
		<field name="description">TEXT[(M)] [CHARACTER SET charset_name] [COLLATE collation_name]

A TEXT column with a maximum length of 65,535 (216 - 1) characters. The
effective maximum length is less if the value contains multi-byte
characters. Each TEXT value is stored using a 2-byte length prefix that
indicates the number of bytes in the value.

An optional length M can be given for this type. If this is done, MySQL
creates the column as the smallest TEXT type large enough to hold
values M characters long.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">164</field>
		<field name="name">CONCAT_WS</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
CONCAT_WS(separator,str1,str2,...)

CONCAT_WS() stands for Concatenate With Separator and is a special form
of CONCAT(). The first argument is the separator for the rest of the
arguments. The separator is added between the strings to be
concatenated. The separator can be a string, as can the rest of the
arguments. If the separator is NULL, the result is NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT CONCAT_WS(',','First name','Second name','Last Name');
        -&gt; 'First name,Second name,Last Name'
mysql&gt; SELECT CONCAT_WS(',','First name',NULL,'Last Name');
        -&gt; 'First name,Last Name'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">165</field>
		<field name="name">ROW_COUNT</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
ROW_COUNT()

Before MySQL 5.5.5, ROW_COUNT() returns the number of rows changed,
deleted, or inserted by the last statement if it was an UPDATE, DELETE,
or INSERT. For other statements, the value may not be meaningful.

As of MySQL 5.5.5, ROW_COUNT() returns a value as follows:

o DDL statements: 0. This applies to statements such as CREATE TABLE or
  DROP TABLE.

o DML statements other than SELECT: The number of affected rows. This
  applies to statements such as UPDATE, INSERT, or DELETE (as before),
  but now also to statements such as ALTER TABLE and LOAD DATA INFILE.

o SELECT: -1 if the statement returns a result set, or the number of
  rows &quot;affected&quot; if it does not. For example, for SELECT * FROM t1,
  ROW_COUNT() returns -1. For SELECT * FROM t1 INTO OUTFILE
  'file_name', ROW_COUNT() returns the number of rows written to the
  file.

o SIGNAL statements: 0.

For UPDATE statements, the affected-rows value by default is the number
of rows actually changed. If you specify the CLIENT_FOUND_ROWS flag to
mysql_real_connect() when connecting to mysqld, the affected-rows value
is the number of rows &quot;found&quot;; that is, matched by the WHERE clause.

For REPLACE statements, the affected-rows value is 2 if the new row
replaced an old row, because in this case, one row was inserted after
the duplicate was deleted.

For INSERT ... ON DUPLICATE KEY UPDATE statements, the affected-rows
value is 1 if the row is inserted as a new row and 2 if an existing row
is updated.

The ROW_COUNT() value is similar to the value from the
mysql_affected_rows() C API function and the row count that the mysql
client displays following statement execution.

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example">mysql&gt; INSERT INTO t VALUES(1),(2),(3);
Query OK, 3 rows affected (0.00 sec)
Records: 3  Duplicates: 0  Warnings: 0

mysql&gt; SELECT ROW_COUNT();
+-------------+
| ROW_COUNT() |
+-------------+
|           3 |
+-------------+
1 row in set (0.00 sec)

mysql&gt; DELETE FROM t WHERE i IN(1,2);
Query OK, 2 rows affected (0.00 sec)

mysql&gt; SELECT ROW_COUNT();
+-------------+
| ROW_COUNT() |
+-------------+
|           2 |
+-------------+
1 row in set (0.00 sec)
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">166</field>
		<field name="name">ASIN</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
ASIN(X)

Returns the arc sine of X, that is, the value whose sine is X. Returns
NULL if X is not in the range -1 to 1.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT ASIN(0.2);
        -&gt; 0.20135792079033
mysql&gt; SELECT ASIN('foo');

+-------------+
| ASIN('foo') |
+-------------+
|           0 |
+-------------+
1 row in set, 1 warning (0.00 sec)

mysql&gt; SHOW WARNINGS;
+---------+------+-----------------------------------------+
| Level   | Code | Message                                 |
+---------+------+-----------------------------------------+
| Warning | 1292 | Truncated incorrect DOUBLE value: 'foo' |
+---------+------+-----------------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">167</field>
		<field name="name">SIGN</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
SIGN(X)

Returns the sign of the argument as -1, 0, or 1, depending on whether X
is negative, zero, or positive.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT SIGN(-32);
        -&gt; -1
mysql&gt; SELECT SIGN(0);
        -&gt; 0
mysql&gt; SELECT SIGN(234);
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">168</field>
		<field name="name">SEC_TO_TIME</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
SEC_TO_TIME(seconds)

Returns the seconds argument, converted to hours, minutes, and seconds,
as a TIME value. The range of the result is constrained to that of the
TIME data type. A warning occurs if the argument corresponds to a value
outside that range.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT SEC_TO_TIME(2378);
        -&gt; '00:39:38'
mysql&gt; SELECT SEC_TO_TIME(2378) + 0;
        -&gt; 3938
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">169</field>
		<field name="name">FLOAT</field>
		<field name="help_category_id">23</field>
		<field name="description">FLOAT[(M,D)] [UNSIGNED] [ZEROFILL]

A small (single-precision) floating-point number. Permissible values
are -3.402823466E+38 to -1.175494351E-38, 0, and 1.175494351E-38 to
3.402823466E+38. These are the theoretical limits, based on the IEEE
standard. The actual range might be slightly smaller depending on your
hardware or operating system.

M is the total number of digits and D is the number of digits following
the decimal point. If M and D are omitted, values are stored to the
limits permitted by the hardware. A single-precision floating-point
number is accurate to approximately 7 decimal places.

UNSIGNED, if specified, disallows negative values.

Using FLOAT might give you some unexpected problems because all
calculations in MySQL are done with double precision. See
http://dev.mysql.com/doc/refman/5.5/en/no-matching-rows.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">170</field>
		<field name="name">LOCATE</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
LOCATE(substr,str), LOCATE(substr,str,pos)

The first syntax returns the position of the first occurrence of
substring substr in string str. The second syntax returns the position
of the first occurrence of substring substr in string str, starting at
position pos. Returns 0 if substr is not in str.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT LOCATE('bar', 'foobarbar');
        -&gt; 4
mysql&gt; SELECT LOCATE('xbar', 'foobar');
        -&gt; 0
mysql&gt; SELECT LOCATE('bar', 'foobarbar', 5);
        -&gt; 7
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">171</field>
		<field name="name">SHOW EVENTS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW EVENTS [{FROM | IN} schema_name]
    [LIKE 'pattern' | WHERE expr]

This statement displays information about Event Manager events. It
requires the EVENT privilege for the database from which the events are
to be shown.

In its simplest form, SHOW EVENTS lists all of the events in the
current schema:

mysql&gt; SELECT CURRENT_USER(), SCHEMA();
+----------------+----------+
| CURRENT_USER() | SCHEMA() |
+----------------+----------+
| jon@ghidora    | myschema |
+----------------+----------+
1 row in set (0.00 sec)

mysql&gt; SHOW EVENTS\G
*************************** 1. row ***************************
                  Db: myschema
                Name: e_daily
             Definer: jon@ghidora
           Time zone: SYSTEM
                Type: RECURRING
          Execute at: NULL
      Interval value: 10
      Interval field: SECOND
              Starts: 2006-02-09 10:41:23
                Ends: NULL
              Status: ENABLED
          Originator: 0
character_set_client: latin1
collation_connection: latin1_swedish_ci
  Database Collation: latin1_swedish_ci

To see events for a specific schema, use the FROM clause. For example,
to see events for the test schema, use the following statement:

SHOW EVENTS FROM test;

The LIKE clause, if present, indicates which event names to match. The
WHERE clause can be given to select rows using more general conditions,
as discussed in
http://dev.mysql.com/doc/refman/5.5/en/extended-show.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-events.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-events.html</field>
	</row>
	<row>
		<field name="help_topic_id">172</field>
		<field name="name">CHARSET</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
CHARSET(str)

Returns the character set of the string argument.

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example">mysql&gt; SELECT CHARSET('abc');
        -&gt; 'latin1'
mysql&gt; SELECT CHARSET(CONVERT('abc' USING utf8));
        -&gt; 'utf8'
mysql&gt; SELECT CHARSET(USER());
        -&gt; 'utf8'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">173</field>
		<field name="name">SUBDATE</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
SUBDATE(date,INTERVAL expr unit), SUBDATE(expr,days)

When invoked with the INTERVAL form of the second argument, SUBDATE()
is a synonym for DATE_SUB(). For information on the INTERVAL unit
argument, see the discussion for DATE_ADD().

mysql&gt; SELECT DATE_SUB('2008-01-02', INTERVAL 31 DAY);
        -&gt; '2007-12-02'
mysql&gt; SELECT SUBDATE('2008-01-02', INTERVAL 31 DAY);
        -&gt; '2007-12-02'

The second form enables the use of an integer value for days. In such
cases, it is interpreted as the number of days to be subtracted from
the date or datetime expression expr.

mysql&gt; SELECT SUBDATE('2008-01-02 12:00:00', 31);
        -&gt; '2007-12-02 12:00:00'

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">174</field>
		<field name="name">DAYOFYEAR</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
DAYOFYEAR(date)

Returns the day of the year for date, in the range 1 to 366.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT DAYOFYEAR('2007-02-03');
        -&gt; 34
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">175</field>
		<field name="name">%</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
N % M, N MOD M

Modulo operation. Returns the remainder of N divided by M. For more
information, see the description for the MOD() function in
http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">176</field>
		<field name="name">LONGTEXT</field>
		<field name="help_category_id">23</field>
		<field name="description">LONGTEXT [CHARACTER SET charset_name] [COLLATE collation_name]

A TEXT column with a maximum length of 4,294,967,295 or 4GB (232 - 1)
characters. The effective maximum length is less if the value contains
multi-byte characters. The effective maximum length of LONGTEXT columns
also depends on the configured maximum packet size in the client/server
protocol and available memory. Each LONGTEXT value is stored using a
4-byte length prefix that indicates the number of bytes in the value.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">177</field>
		<field name="name">KILL</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
KILL [CONNECTION | QUERY] thread_id

Each connection to mysqld runs in a separate thread. You can see which
threads are running with the SHOW PROCESSLIST statement and kill a
thread with the KILL thread_id statement.

KILL permits an optional CONNECTION or QUERY modifier:

o KILL CONNECTION is the same as KILL with no modifier: It terminates
  the connection associated with the given thread_id.

o KILL QUERY terminates the statement that the connection is currently
  executing, but leaves the connection itself intact.

If you have the PROCESS privilege, you can see all threads. If you have
the SUPER privilege, you can kill all threads and statements.
Otherwise, you can see and kill only your own threads and statements.

You can also use the mysqladmin processlist and mysqladmin kill
commands to examine and kill threads.

*Note*: You cannot use KILL with the Embedded MySQL Server library
because the embedded server merely runs inside the threads of the host
application. It does not create any connection threads of its own.

URL: http://dev.mysql.com/doc/refman/5.5/en/kill.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/kill.html</field>
	</row>
	<row>
		<field name="help_topic_id">178</field>
		<field name="name">DISJOINT</field>
		<field name="help_category_id">31</field>
		<field name="description">Disjoint(g1,g2)

Returns 1 or 0 to indicate whether g1 is spatially disjoint from (does
not intersect) g2.

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">179</field>
		<field name="name">ASTEXT</field>
		<field name="help_category_id">3</field>
		<field name="description">AsText(g), AsWKT(g)

Converts a value in internal geometry format to its WKT representation
and returns the string result.

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-to-convert-geometries-between-formats.html

</field>
		<field name="example">mysql&gt; SET @g = 'LineString(1 1,2 2,3 3)';
mysql&gt; SELECT AsText(GeomFromText(@g));
+--------------------------+
| AsText(GeomFromText(@g)) |
+--------------------------+
| LINESTRING(1 1,2 2,3 3)  |
+--------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-to-convert-geometries-between-formats.html</field>
	</row>
	<row>
		<field name="help_topic_id">180</field>
		<field name="name">LPAD</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
LPAD(str,len,padstr)

Returns the string str, left-padded with the string padstr to a length
of len characters. If str is longer than len, the return value is
shortened to len characters.


URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT LPAD('hi',4,'??');
        -&gt; '??hi'
mysql&gt; SELECT LPAD('hi',1,'??');
        -&gt; 'h'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">181</field>
		<field name="name">DECLARE CONDITION</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
DECLARE condition_name CONDITION FOR condition_value

condition_value:
    mysql_error_code
  | SQLSTATE [VALUE] sqlstate_value

The DECLARE ... CONDITION statement declares a named error condition,
associating a name with a condition that needs specific handling. The
name can be referred to in a subsequent DECLARE ... HANDLER statement
(see [HELP DECLARE HANDLER]).

Condition declarations must appear before cursor or handler
declarations.

The condition_value for DECLARE ... CONDITION can be a MySQL error code
(a number) or an SQLSTATE value (a 5-character string literal). You
should not use MySQL error code 0 or SQLSTATE values that begin with
'00', because those indicate success rather than an error condition.
For a list of MySQL error codes and SQLSTATE values, see
http://dev.mysql.com/doc/refman/5.5/en/error-messages-server.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/declare-condition.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/declare-condition.html</field>
	</row>
	<row>
		<field name="help_topic_id">182</field>
		<field name="name">OVERLAPS</field>
		<field name="help_category_id">31</field>
		<field name="description">Overlaps(g1,g2)

Returns 1 or 0 to indicate whether g1 spatially overlaps g2. The term
spatially overlaps is used if two geometries intersect and their
intersection results in a geometry of the same dimension but not equal
to either of the given geometries.

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">183</field>
		<field name="name">SET GLOBAL SQL_SLAVE_SKIP_COUNTER</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
SET GLOBAL sql_slave_skip_counter = N

This statement skips the next N events from the master. This is useful
for recovering from replication stops caused by a statement.

This statement is valid only when the slave threads are not running.
Otherwise, it produces an error.

URL: http://dev.mysql.com/doc/refman/5.5/en/set-global-sql-slave-skip-counter.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/set-global-sql-slave-skip-counter.html</field>
	</row>
	<row>
		<field name="help_topic_id">184</field>
		<field name="name">NUMGEOMETRIES</field>
		<field name="help_category_id">26</field>
		<field name="description">NumGeometries(gc)

Returns the number of geometries in the GeometryCollection value gc.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SET @gc = 'GeometryCollection(Point(1 1),LineString(2 2, 3 3))';
mysql&gt; SELECT NumGeometries(GeomFromText(@gc));
+----------------------------------+
| NumGeometries(GeomFromText(@gc)) |
+----------------------------------+
|                                2 |
+----------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">185</field>
		<field name="name">MONTHNAME</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
MONTHNAME(date)

Returns the full name of the month for date. The language used for the
name is controlled by the value of the lc_time_names system variable
(http://dev.mysql.com/doc/refman/5.5/en/locale-support.html).

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT MONTHNAME('2008-02-03');
        -&gt; 'February'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">186</field>
		<field name="name">CHANGE MASTER TO</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
CHANGE MASTER TO option [, option] ...

option:
    MASTER_BIND = 'interface_name'
  | MASTER_HOST = 'host_name'
  | MASTER_USER = 'user_name'
  | MASTER_PASSWORD = 'password'
  | MASTER_PORT = port_num
  | MASTER_CONNECT_RETRY = interval
  | MASTER_HEARTBEAT_PERIOD = interval
  | MASTER_LOG_FILE = 'master_log_name'
  | MASTER_LOG_POS = master_log_pos
  | RELAY_LOG_FILE = 'relay_log_name'
  | RELAY_LOG_POS = relay_log_pos
  | MASTER_SSL = {0|1}
  | MASTER_SSL_CA = 'ca_file_name'
  | MASTER_SSL_CAPATH = 'ca_directory_name'
  | MASTER_SSL_CERT = 'cert_file_name'
  | MASTER_SSL_KEY = 'key_file_name'
  | MASTER_SSL_CIPHER = 'cipher_list'
  | MASTER_SSL_VERIFY_SERVER_CERT = {0|1}
  | IGNORE_SERVER_IDS = (server_id_list)

server_id_list:
    [server_id [, server_id] ... ]

CHANGE MASTER TO changes the parameters that the slave server uses for
connecting to the master server, for reading the master binary log, and
reading the slave relay log. It also updates the contents of the
master.info and relay-log.info files. To use CHANGE MASTER TO, the
slave replication threads must be stopped (use STOP SLAVE if
necessary).

Options not specified retain their value, except as indicated in the
following discussion. Thus, in most cases, there is no need to specify
options that do not change. For example, if the password to connect to
your MySQL master has changed, you just need to issue these statements
to tell the slave about the new password:

STOP SLAVE; -- if replication was running
CHANGE MASTER TO MASTER_PASSWORD='new3cret';
START SLAVE; -- if you want to restart replication

MASTER_HOST, MASTER_USER, MASTER_PASSWORD, and MASTER_PORT provide
information to the slave about how to connect to its master:

o MASTER_HOST and MASTER_PORT are the host name (or IP address) of the
  master host and its TCP/IP port.

  *Note*: Replication cannot use Unix socket files. You must be able to
  connect to the master MySQL server using TCP/IP.

  If you specify the MASTER_HOST or MASTER_PORT option, the slave
  assumes that the master server is different from before (even if the
  option value is the same as its current value.) In this case, the old
  values for the master binary log file name and position are
  considered no longer applicable, so if you do not specify
  MASTER_LOG_FILE and MASTER_LOG_POS in the statement,
  MASTER_LOG_FILE='' and MASTER_LOG_POS=4 are silently appended to it.

  Setting MASTER_HOST='' (that is, setting its value explicitly to an
  empty string) is not the same as not setting MASTER_HOST at all.
  Beginning with MySQL 5.5, trying to set MASTER_HOST to an empty
  string fails with an error. Previously, setting MASTER_HOST to an
  empty string caused START SLAVE subsequently to fail. (Bug #28796)

o MASTER_USER and MASTER_PASSWORD are the user name and password of the
  account to use for connecting to the master.

  In MySQL 5.5.20 and later, MASTER_USER cannot be made empty; setting
  MASTER_USER = '' or leaving it unset when setting a value for for
  MASTER_PASSWORD causes an error (Bug #13427949).

  Currently, a password used for a replication slave account is
  effectively limited to 32 characters in length; the password can be
  longer, but any excess characters are truncated. This is not due to
  any limit imposed by the MySQL Server generally, but rather is an
  issue specific to MySQL Replication. (For more information, see Bug
  #43439.)

  The text of a running CHANGE MASTER TO statement, including values
  for MASTER_USER and MASTER_PASSWORD, can be seen in the output of a
  concurrent SHOW PROCESSLIST statement.

The MASTER_SSL_xxx options provide information about using SSL for the
connection. They correspond to the --ssl-xxx options described in
http://dev.mysql.com/doc/refman/5.5/en/ssl-options.html, and
http://dev.mysql.com/doc/refman/5.5/en/replication-solutions-ssl.html.
These options can be changed even on slaves that are compiled without
SSL support. They are saved to the master.info file, but are ignored if
the slave does not have SSL support enabled.

MASTER_CONNECT_RETRY specifies how many seconds to wait between connect
retries. The default is 60. The number of reconnection attempts is
limited by the --master-retry-count server option; for more
information, see
http://dev.mysql.com/doc/refman/5.5/en/replication-options.html.

The MASTER_BIND option is available in MySQL Cluster NDB 7.2 and later,
but is not supported in mainline MySQL 5.5.

MASTER_BIND is for use on replication slaves having multiple network
interfaces, and determines which of the slave's network interfaces is
chosen for connecting to the master.

MASTER_HEARTBEAT_PERIOD sets the interval in seconds between
replication heartbeats. Whenever the master's binary log is updated
with an event, the waiting period for the next heartbeat is reset.
interval is a decimal value having the range 0 to 4294967 seconds and a
resolution in milliseconds; the smallest nonzero value is 0.001.
Heartbeats are sent by the master only if there are no unsent events in
the binary log file for a period longer than interval.

Setting interval to 0 disables heartbeats altogether. The default value
for interval is equal to the value of slave_net_timeout divided by 2.

Setting @@global.slave_net_timeout to a value less than that of the
current heartbeat interval results in a warning being issued. The
effect of issuing RESET SLAVE on the heartbeat interval is to reset it
to the default value.

MASTER_LOG_FILE and MASTER_LOG_POS are the coordinates at which the
slave I/O thread should begin reading from the master the next time the
thread starts. RELAY_LOG_FILE and RELAY_LOG_POS are the coordinates at
which the slave SQL thread should begin reading from the relay log the
next time the thread starts. If you specify either of MASTER_LOG_FILE
or MASTER_LOG_POS, you cannot specify RELAY_LOG_FILE or RELAY_LOG_POS.
If neither of MASTER_LOG_FILE or MASTER_LOG_POS is specified, the slave
uses the last coordinates of the slave SQL thread before CHANGE MASTER
TO was issued. This ensures that there is no discontinuity in
replication, even if the slave SQL thread was late compared to the
slave I/O thread, when you merely want to change, say, the password to
use.

CHANGE MASTER TO deletes all relay log files and starts a new one,
unless you specify RELAY_LOG_FILE or RELAY_LOG_POS. In that case, relay
log files are kept; the relay_log_purge global variable is set silently
to 0.

Prior to MySQL 5.5, RELAY_LOG_FILE required an absolute path. In MySQL
5.5, the path can be relative, in which case the path is assumed to be
relative to the slave's data directory. (Bug #12190)

IGNORE_SERVER_IDS was added in MySQL 5.5. This option takes a
comma-separated list of 0 or more server IDs. Events originating from
the corresponding servers are ignored, with the exception of log
rotation and deletion events, which are still recorded in the relay
log.

In circular replication, the originating server normally acts as the
terminator of its own events, so that they are not applied more than
once. Thus, this option is useful in circular replication when one of
the servers in the circle is removed. Suppose that you have a circular
replication setup with 4 servers, having server IDs 1, 2, 3, and 4, and
server 3 fails. When bridging the gap by starting replication from
server 2 to server 4, you can include IGNORE_SERVER_IDS = (3) in the
CHANGE MASTER TO statement that you issue on server 4 to tell it to use
server 2 as its master instead of server 3. Doing so causes it to
ignore and not to propagate any statements that originated with the
server that is no longer in use.

If a CHANGE MASTER TO statement is issued without any IGNORE_SERVER_IDS
option, any existing list is preserved; RESET SLAVE also has no effect
on the server ID list. To clear the list of ignored servers, it is
necessary to use the option with an empty list:

CHANGE MASTER TO IGNORE_SERVER_IDS = ();

If IGNORE_SERVER_IDS contains the server's own ID and the server was
started with the --replicate-same-server-id option enabled, an error
results.

Also beginning with MySQL 5.5, the master.info file and the output of
SHOW SLAVE STATUS are extended to provide the list of servers that are
currently ignored. For more information, see
http://dev.mysql.com/doc/refman/5.5/en/slave-logs-status.html, and
[HELP SHOW SLAVE STATUS].

Beginning with MySQL 5.5.5, invoking CHANGE MASTER TO causes the
previous values for MASTER_HOST, MASTER_PORT, MASTER_LOG_FILE, and
MASTER_LOG_POS to be written to the error log, along with other
information about the slave's state prior to execution.

CHANGE MASTER TO is useful for setting up a slave when you have the
snapshot of the master and have recorded the master binary log
coordinates corresponding to the time of the snapshot. After loading
the snapshot into the slave to synchronize it to the slave, you can run
CHANGE MASTER TO MASTER_LOG_FILE='log_name', MASTER_LOG_POS=log_pos on
the slave to specify the coordinates at which the slave should begin
reading the master binary log.

The following example changes the master server the slave uses and
establishes the master binary log coordinates from which the slave
begins reading. This is used when you want to set up the slave to
replicate the master:

CHANGE MASTER TO
  MASTER_HOST='master2.mycompany.com',
  MASTER_USER='replication',
  MASTER_PASSWORD='bigs3cret',
  MASTER_PORT=3306,
  MASTER_LOG_FILE='master2-bin.001',
  MASTER_LOG_POS=4,
  MASTER_CONNECT_RETRY=10;

The next example shows an operation that is less frequently employed.
It is used when the slave has relay log files that you want it to
execute again for some reason. To do this, the master need not be
reachable. You need only use CHANGE MASTER TO and start the SQL thread
(START SLAVE SQL_THREAD):

CHANGE MASTER TO
  RELAY_LOG_FILE='slave-relay-bin.006',
  RELAY_LOG_POS=4025;

URL: http://dev.mysql.com/doc/refman/5.5/en/change-master-to.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/change-master-to.html</field>
	</row>
	<row>
		<field name="help_topic_id">187</field>
		<field name="name">DROP DATABASE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
DROP {DATABASE | SCHEMA} [IF EXISTS] db_name

DROP DATABASE drops all tables in the database and deletes the
database. Be very careful with this statement! To use DROP DATABASE,
you need the DROP privilege on the database. DROP SCHEMA is a synonym
for DROP DATABASE.

*Important*: When a database is dropped, user privileges on the
database are not automatically dropped. See [HELP GRANT].

IF EXISTS is used to prevent an error from occurring if the database
does not exist.

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-database.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-database.html</field>
	</row>
	<row>
		<field name="help_topic_id">188</field>
		<field name="name">MBREQUAL</field>
		<field name="help_category_id">6</field>
		<field name="description">MBREqual(g1,g2)

Returns 1 or 0 to indicate whether the Minimum Bounding Rectangles of
the two geometries g1 and g2 are the same.

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">189</field>
		<field name="name">TIMESTAMP FUNCTION</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
TIMESTAMP(expr), TIMESTAMP(expr1,expr2)

With a single argument, this function returns the date or datetime
expression expr as a datetime value. With two arguments, it adds the
time expression expr2 to the date or datetime expression expr1 and
returns the result as a datetime value.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT TIMESTAMP('2003-12-31');
        -&gt; '2003-12-31 00:00:00'
mysql&gt; SELECT TIMESTAMP('2003-12-31 12:00:00','12:00:00');
        -&gt; '2004-01-01 00:00:00'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">190</field>
		<field name="name">PROCEDURE ANALYSE</field>
		<field name="help_category_id">34</field>
		<field name="description">Syntax:
ANALYSE([max_elements[,max_memory]])

ANALYSE() examines the result from a query and returns an analysis of
the results that suggests optimal data types for each column that may
help reduce table sizes. To obtain this analysis, append PROCEDURE
ANALYSE to the end of a SELECT statement:

SELECT ... FROM ... WHERE ... PROCEDURE ANALYSE([max_elements,[max_memory]])

For example:

SELECT col1, col2 FROM table1 PROCEDURE ANALYSE(10, 2000);

The results show some statistics for the values returned by the query,
and propose an optimal data type for the columns. This can be helpful
for checking your existing tables, or after importing new data. You may
need to try different settings for the arguments so that PROCEDURE
ANALYSE() does not suggest the ENUM data type when it is not
appropriate.

The arguments are optional and are used as follows:

o max_elements (default 256) is the maximum number of distinct values
  that ANALYSE() notices per column. This is used by ANALYSE() to check
  whether the optimal data type should be of type ENUM; if there are
  more than max_elements distinct values, then ENUM is not a suggested
  type.

o max_memory (default 8192) is the maximum amount of memory that
  ANALYSE() should allocate per column while trying to find all
  distinct values.

URL: http://dev.mysql.com/doc/refman/5.5/en/procedure-analyse.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/procedure-analyse.html</field>
	</row>
	<row>
		<field name="help_topic_id">191</field>
		<field name="name">HELP_VERSION</field>
		<field name="help_category_id">9</field>
		<field name="description">This help information was generated from the MySQL 5.5 Reference Manual
on: 2013-03-22 (revision: 34719)

This information applies to MySQL 5.5 through 5.5.32.
</field>
		<field name="example"></field>
		<field name="url"></field>
	</row>
	<row>
		<field name="help_topic_id">192</field>
		<field name="name">CHARACTER_LENGTH</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
CHARACTER_LENGTH(str)

CHARACTER_LENGTH() is a synonym for CHAR_LENGTH().

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">193</field>
		<field name="name">SHOW GRANTS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW GRANTS [FOR user]

This statement lists the GRANT statement or statements that must be
issued to duplicate the privileges that are granted to a MySQL user
account. The account is named using the same format as for the GRANT
statement; for example, 'jeffrey'@'localhost'. If you specify only the
user name part of the account name, a host name part of '%' is used.
For additional information about specifying account names, see [HELP
GRANT].

mysql&gt; SHOW GRANTS FOR 'root'@'localhost';
+---------------------------------------------------------------------+
| Grants for root@localhost                                           |
+---------------------------------------------------------------------+
| GRANT ALL PRIVILEGES ON *.* TO 'root'@'localhost' WITH GRANT OPTION |
+---------------------------------------------------------------------+

To list the privileges granted to the account that you are using to
connect to the server, you can use any of the following statements:

SHOW GRANTS;
SHOW GRANTS FOR CURRENT_USER;
SHOW GRANTS FOR CURRENT_USER();

If SHOW GRANTS FOR CURRENT_USER (or any of the equivalent syntaxes) is
used in DEFINER context, such as within a stored procedure that is
defined with SQL SECURITY DEFINER), the grants displayed are those of
the definer and not the invoker.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-grants.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-grants.html</field>
	</row>
	<row>
		<field name="help_topic_id">194</field>
		<field name="name">SHOW PRIVILEGES</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW PRIVILEGES

SHOW PRIVILEGES shows the list of system privileges that the MySQL
server supports. The exact list of privileges depends on the version of
your server.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-privileges.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-privileges.html</field>
	</row>
	<row>
		<field name="help_topic_id">195</field>
		<field name="name">CREATE TABLESPACE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
CREATE TABLESPACE tablespace_name
    ADD DATAFILE 'file_name'
    USE LOGFILE GROUP logfile_group
    [EXTENT_SIZE [=] extent_size]
    [INITIAL_SIZE [=] initial_size]
    [AUTOEXTEND_SIZE [=] autoextend_size]
    [MAX_SIZE [=] max_size]
    [NODEGROUP [=] nodegroup_id]
    [WAIT]
    [COMMENT [=] comment_text]
    ENGINE [=] engine_name

This statement is used to create a tablespace, which can contain one or
more data files, providing storage space for tables. One data file is
created and added to the tablespace using this statement. Additional
data files may be added to the tablespace by using the ALTER TABLESPACE
statement (see [HELP ALTER TABLESPACE]). For rules covering the naming
of tablespaces, see
http://dev.mysql.com/doc/refman/5.5/en/identifiers.html.

*Note*: All MySQL Cluster Disk Data objects share the same namespace.
This means that each Disk Data object must be uniquely named (and not
merely each Disk Data object of a given type). For example, you cannot
have a tablespace and a log file group with the same name, or a
tablespace and a data file with the same name.

A log file group of one or more UNDO log files must be assigned to the
tablespace to be created with the USE LOGFILE GROUP clause.
logfile_group must be an existing log file group created with CREATE
LOGFILE GROUP (see [HELP CREATE LOGFILE GROUP]). Multiple tablespaces
may use the same log file group for UNDO logging.

The EXTENT_SIZE sets the size, in bytes, of the extents used by any
files belonging to the tablespace. The default value is 1M. The minimum
size is 32K, and theoretical maximum is 2G, although the practical
maximum size depends on a number of factors. In most cases, changing
the extent size does not have any measurable effect on performance, and
the default value is recommended for all but the most unusual
situations.

An extent is a unit of disk space allocation. One extent is filled with
as much data as that extent can contain before another extent is used.
In theory, up to 65,535 (64K) extents may used per data file; however,
the recommended maximum is 32,768 (32K). The recommended maximum size
for a single data file is 32G---that is, 32K extents x 1 MB per extent.
In addition, once an extent is allocated to a given partition, it
cannot be used to store data from a different partition; an extent
cannot store data from more than one partition. This means, for example
that a tablespace having a single datafile whose INITIAL_SIZE is 256 MB
and whose EXTENT_SIZE is 128M has just two extents, and so can be used
to store data from at most two different disk data table partitions.

You can see how many extents remain free in a given data file by
querying the INFORMATION_SCHEMA.FILES table, and so derive an estimate
for how much space remains free in the file. For further discussion and
examples, see http://dev.mysql.com/doc/refman/5.5/en/files-table.html.

The INITIAL_SIZE parameter sets the data file's total size in bytes.
Once the file has been created, its size cannot be changed; however,
you can add more data files to the tablespace using ALTER TABLESPACE
... ADD DATAFILE. See [HELP ALTER TABLESPACE].

INITIAL_SIZE is optional; its default value is 128M.

On 32-bit systems, the maximum supported value for INITIAL_SIZE is 4G.
(Bug #29186)

When setting EXTENT_SIZE or INITIAL_SIZE (either or both), you may
optionally follow the number with a one-letter abbreviation for an
order of magnitude, similar to those used in my.cnf. Generally, this is
one of the letters M (for megabytes) or G (for gigabytes).

INITIAL_SIZE, EXTENT_SIZE, and UNDO_BUFFER_SIZE are subject to rounding
as follows:

o EXTENT_SIZE and UNDO_BUFFER_SIZE are each rounded up to the nearest
  whole multiple of 32K.

o INITIAL_SIZE is rounded down to the nearest whole multiple of 32K.

  For data files, INITIAL_SIZE is subject to further rounding; the
  result just obtained is rounded up to the nearest whole multiple of
  EXTENT_SIZE (after any rounding).

The rounding just described is done explicitly, and a warning is issued
by the MySQL Server when any such rounding is performed. The rounded
values are also used by the NDB kernel for calculating
INFORMATION_SCHEMA.FILES column values and other purposes. However, to
avoid an unexpected result, we suggest that you always use whole
multiples of 32K in specifying these options.

AUTOEXTEND_SIZE, MAX_SIZE, NODEGROUP, WAIT, and COMMENT are parsed but
ignored, and so currently have no effect. These options are intended
for future expansion.

The ENGINE parameter determines the storage engine which uses this
tablespace, with engine_name being the name of the storage engine.
Currently, engine_name must be one of the values NDB or NDBCLUSTER.

When CREATE TABLESPACE is used with ENGINE = NDB, a tablespace and
associated data file are created on each Cluster data node. You can
verify that the data files were created and obtain information about
them by querying the INFORMATION_SCHEMA.FILES table. For example:

mysql&gt; SELECT LOGFILE_GROUP_NAME, FILE_NAME, EXTRA
    -&gt; FROM INFORMATION_SCHEMA.FILES
    -&gt; WHERE TABLESPACE_NAME = 'newts' AND FILE_TYPE = 'DATAFILE';
+--------------------+-------------+----------------+
| LOGFILE_GROUP_NAME | FILE_NAME   | EXTRA          |
+--------------------+-------------+----------------+
| lg_3               | newdata.dat | CLUSTER_NODE=3 |
| lg_3               | newdata.dat | CLUSTER_NODE=4 |
+--------------------+-------------+----------------+
2 rows in set (0.01 sec)

(See http://dev.mysql.com/doc/refman/5.5/en/files-table.html.)

CREATE TABLESPACE is useful only with Disk Data storage for MySQL
Cluster. See
http://dev.mysql.com/doc/refman/5.5/en/mysql-cluster-disk-data.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/create-tablespace.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-tablespace.html</field>
	</row>
	<row>
		<field name="help_topic_id">196</field>
		<field name="name">INSERT FUNCTION</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
INSERT(str,pos,len,newstr)

Returns the string str, with the substring beginning at position pos
and len characters long replaced by the string newstr. Returns the
original string if pos is not within the length of the string. Replaces
the rest of the string from position pos if len is not within the
length of the rest of the string. Returns NULL if any argument is NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT INSERT('Quadratic', 3, 4, 'What');
        -&gt; 'QuWhattic'
mysql&gt; SELECT INSERT('Quadratic', -1, 4, 'What');
        -&gt; 'Quadratic'
mysql&gt; SELECT INSERT('Quadratic', 3, 100, 'What');
        -&gt; 'QuWhat'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">197</field>
		<field name="name">CRC32</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
CRC32(expr)

Computes a cyclic redundancy check value and returns a 32-bit unsigned
value. The result is NULL if the argument is NULL. The argument is
expected to be a string and (if possible) is treated as one if it is
not.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT CRC32('MySQL');
        -&gt; 3259397556
mysql&gt; SELECT CRC32('mysql');
        -&gt; 2501908538
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">198</field>
		<field name="name">XOR</field>
		<field name="help_category_id">15</field>
		<field name="description">Syntax:
XOR

Logical XOR. Returns NULL if either operand is NULL. For non-NULL
operands, evaluates to 1 if an odd number of operands is nonzero,
otherwise 0 is returned.

URL: http://dev.mysql.com/doc/refman/5.5/en/logical-operators.html

</field>
		<field name="example">mysql&gt; SELECT 1 XOR 1;
        -&gt; 0
mysql&gt; SELECT 1 XOR 0;
        -&gt; 1
mysql&gt; SELECT 1 XOR NULL;
        -&gt; NULL
mysql&gt; SELECT 1 XOR 1 XOR 1;
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/logical-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">199</field>
		<field name="name">STARTPOINT</field>
		<field name="help_category_id">13</field>
		<field name="description">StartPoint(ls)

Returns the Point that is the start point of the LineString value ls.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SET @ls = 'LineString(1 1,2 2,3 3)';
mysql&gt; SELECT AsText(StartPoint(GeomFromText(@ls)));
+---------------------------------------+
| AsText(StartPoint(GeomFromText(@ls))) |
+---------------------------------------+
| POINT(1 1)                            |
+---------------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">200</field>
		<field name="name">GRANT</field>
		<field name="help_category_id">10</field>
		<field name="description">Syntax:
GRANT
    priv_type [(column_list)]
      [, priv_type [(column_list)]] ...
    ON [object_type] priv_level
    TO user_specification [, user_specification] ...
    [REQUIRE {NONE | ssl_option [[AND] ssl_option] ...}]
    [WITH with_option ...]

GRANT PROXY ON user_specification
    TO user_specification [, user_specification] ...
    [WITH GRANT OPTION]

object_type:
    TABLE
  | FUNCTION
  | PROCEDURE

priv_level:
    *
  | *.*
  | db_name.*
  | db_name.tbl_name
  | tbl_name
  | db_name.routine_name

user_specification:
    user
    [
        IDENTIFIED BY [PASSWORD] 'password'
      | IDENTIFIED WITH auth_plugin [AS 'auth_string']
    ]

ssl_option:
    SSL
  | X509
  | CIPHER 'cipher'
  | ISSUER 'issuer'
  | SUBJECT 'subject'

with_option:
    GRANT OPTION
  | MAX_QUERIES_PER_HOUR count
  | MAX_UPDATES_PER_HOUR count
  | MAX_CONNECTIONS_PER_HOUR count
  | MAX_USER_CONNECTIONS count

The GRANT statement grants privileges to MySQL user accounts. GRANT
also serves to specify other account characteristics such as use of
secure connections and limits on access to server resources. To use
GRANT, you must have the GRANT OPTION privilege, and you must have the
privileges that you are granting.

Normally, a database administrator first uses CREATE USER to create an
account, then GRANT to define its privileges and characteristics. For
example:

CREATE USER 'jeffrey'@'localhost' IDENTIFIED BY 'mypass';
GRANT ALL ON db1.* TO 'jeffrey'@'localhost';
GRANT SELECT ON db2.invoice TO 'jeffrey'@'localhost';
GRANT USAGE ON *.* TO 'jeffrey'@'localhost' WITH MAX_QUERIES_PER_HOUR 90;

However, if an account named in a GRANT statement does not already
exist, GRANT may create it under the conditions described later in the
discussion of the NO_AUTO_CREATE_USER SQL mode.

The REVOKE statement is related to GRANT and enables administrators to
remove account privileges. See [HELP REVOKE].

When successfully executed from the mysql program, GRANT responds with
Query OK, 0 rows affected. To determine what privileges result from the
operation, use SHOW GRANTS. See [HELP SHOW GRANTS].

URL: http://dev.mysql.com/doc/refman/5.5/en/grant.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/grant.html</field>
	</row>
	<row>
		<field name="help_topic_id">201</field>
		<field name="name">DECLARE VARIABLE</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
DECLARE var_name [, var_name] ... type [DEFAULT value]

This statement declares local variables within stored programs. To
provide a default value for a variable, include a DEFAULT clause. The
value can be specified as an expression; it need not be a constant. If
the DEFAULT clause is missing, the initial value is NULL.

Local variables are treated like stored routine parameters with respect
to data type and overflow checking. See [HELP CREATE PROCEDURE].

Variable declarations must appear before cursor or handler
declarations.

Local variable names are not case sensitive. Permissible characters and
quoting rules are the same as for other identifiers, as described in
http://dev.mysql.com/doc/refman/5.5/en/identifiers.html.

The scope of a local variable is the BEGIN ... END block within which
it is declared. The variable can be referred to in blocks nested within
the declaring block, except those blocks that declare a variable with
the same name.

URL: http://dev.mysql.com/doc/refman/5.5/en/declare-local-variable.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/declare-local-variable.html</field>
	</row>
	<row>
		<field name="help_topic_id">202</field>
		<field name="name">MPOLYFROMTEXT</field>
		<field name="help_category_id">3</field>
		<field name="description">MPolyFromText(wkt[,srid]), MultiPolygonFromText(wkt[,srid])

Constructs a MULTIPOLYGON value using its WKT representation and SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">203</field>
		<field name="name">MBRINTERSECTS</field>
		<field name="help_category_id">6</field>
		<field name="description">MBRIntersects(g1,g2)

Returns 1 or 0 to indicate whether the Minimum Bounding Rectangles of
the two geometries g1 and g2 intersect.

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">204</field>
		<field name="name">BIT_OR</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
BIT_OR(expr)

Returns the bitwise OR of all bits in expr. The calculation is
performed with 64-bit (BIGINT) precision.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">205</field>
		<field name="name">YEARWEEK</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
YEARWEEK(date), YEARWEEK(date,mode)

Returns year and week for a date. The mode argument works exactly like
the mode argument to WEEK(). The year in the result may be different
from the year in the date argument for the first and the last week of
the year.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT YEARWEEK('1987-01-01');
        -&gt; 198653
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">206</field>
		<field name="name">NOT BETWEEN</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
expr NOT BETWEEN min AND max

This is the same as NOT (expr BETWEEN min AND max).

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">207</field>
		<field name="name">IS NOT</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
IS NOT boolean_value

Tests a value against a boolean value, where boolean_value can be TRUE,
FALSE, or UNKNOWN.

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT 1 IS NOT UNKNOWN, 0 IS NOT UNKNOWN, NULL IS NOT UNKNOWN;
        -&gt; 1, 1, 0
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">208</field>
		<field name="name">LOG10</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
LOG10(X)

Returns the base-10 logarithm of X.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT LOG10(2);
        -&gt; 0.30102999566398
mysql&gt; SELECT LOG10(100);
        -&gt; 2
mysql&gt; SELECT LOG10(-100);
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">209</field>
		<field name="name">SQRT</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
SQRT(X)

Returns the square root of a nonnegative number X.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT SQRT(4);
        -&gt; 2
mysql&gt; SELECT SQRT(20);
        -&gt; 4.4721359549996
mysql&gt; SELECT SQRT(-16);
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">210</field>
		<field name="name">DECIMAL</field>
		<field name="help_category_id">23</field>
		<field name="description">DECIMAL[(M[,D])] [UNSIGNED] [ZEROFILL]

A packed &quot;exact&quot; fixed-point number. M is the total number of digits
(the precision) and D is the number of digits after the decimal point
(the scale). The decimal point and (for negative numbers) the &quot;-&quot; sign
are not counted in M. If D is 0, values have no decimal point or
fractional part. The maximum number of digits (M) for DECIMAL is 65.
The maximum number of supported decimals (D) is 30. If D is omitted,
the default is 0. If M is omitted, the default is 10.

UNSIGNED, if specified, disallows negative values.

All basic calculations (+, -, *, /) with DECIMAL columns are done with
a precision of 65 digits.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">211</field>
		<field name="name">CREATE INDEX</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
CREATE [ONLINE|OFFLINE] [UNIQUE|FULLTEXT|SPATIAL] INDEX index_name
    [index_type]
    ON tbl_name (index_col_name,...)
    [index_option] ...

index_col_name:
    col_name [(length)] [ASC | DESC]

index_type:
    USING {BTREE | HASH}

index_option:
    KEY_BLOCK_SIZE [=] value
  | index_type
  | WITH PARSER parser_name
  | COMMENT 'string'

CREATE INDEX is mapped to an ALTER TABLE statement to create indexes.
See [HELP ALTER TABLE]. CREATE INDEX cannot be used to create a PRIMARY
KEY; use ALTER TABLE instead. For more information about indexes, see
http://dev.mysql.com/doc/refman/5.5/en/mysql-indexes.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/create-index.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-index.html</field>
	</row>
	<row>
		<field name="help_topic_id">212</field>
		<field name="name">CREATE FUNCTION</field>
		<field name="help_category_id">40</field>
		<field name="description">The CREATE FUNCTION statement is used to create stored functions and
user-defined functions (UDFs):

o For information about creating stored functions, see [HELP CREATE
  PROCEDURE].

o For information about creating user-defined functions, see [HELP
  CREATE FUNCTION UDF].

URL: http://dev.mysql.com/doc/refman/5.5/en/create-function.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-function.html</field>
	</row>
	<row>
		<field name="help_topic_id">213</field>
		<field name="name">ALTER DATABASE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
ALTER {DATABASE | SCHEMA} [db_name]
    alter_specification ...
ALTER {DATABASE | SCHEMA} db_name
    UPGRADE DATA DIRECTORY NAME

alter_specification:
    [DEFAULT] CHARACTER SET [=] charset_name
  | [DEFAULT] COLLATE [=] collation_name

ALTER DATABASE enables you to change the overall characteristics of a
database. These characteristics are stored in the db.opt file in the
database directory. To use ALTER DATABASE, you need the ALTER privilege
on the database. ALTER SCHEMA is a synonym for ALTER DATABASE.

The database name can be omitted from the first syntax, in which case
the statement applies to the default database.

National Language Characteristics

The CHARACTER SET clause changes the default database character set.
The COLLATE clause changes the default database collation.
http://dev.mysql.com/doc/refman/5.5/en/charset.html, discusses
character set and collation names.

You can see what character sets and collations are available using,
respectively, the SHOW CHARACTER SET and SHOW COLLATION statements. See
[HELP SHOW CHARACTER SET], and [HELP SHOW COLLATION], for more
information.

If you change the default character set or collation for a database,
stored routines that use the database defaults must be dropped and
recreated so that they use the new defaults. (In a stored routine,
variables with character data types use the database defaults if the
character set or collation are not specified explicitly. See [HELP
CREATE PROCEDURE].)

Upgrading from Versions Older than MySQL 5.1

The syntax that includes the UPGRADE DATA DIRECTORY NAME clause updates
the name of the directory associated with the database to use the
encoding implemented in MySQL 5.1 for mapping database names to
database directory names (see
http://dev.mysql.com/doc/refman/5.5/en/identifier-mapping.html). This
clause is for use under these conditions:

o It is intended when upgrading MySQL to 5.1 or later from older
  versions.

o It is intended to update a database directory name to the current
  encoding format if the name contains special characters that need
  encoding.

o The statement is used by mysqlcheck (as invoked by mysql_upgrade).

For example, if a database in MySQL 5.0 has the name a-b-c, the name
contains instances of the - (dash) character. In MySQL 5.0, the
database directory is also named a-b-c, which is not necessarily safe
for all file systems. In MySQL 5.1 and later, the same database name is
encoded as a@002db@002dc to produce a file system-neutral directory
name.

When a MySQL installation is upgraded to MySQL 5.1 or later from an
older version,the server displays a name such as a-b-c (which is in the
old format) as #mysql50#a-b-c, and you must refer to the name using the
#mysql50# prefix. Use UPGRADE DATA DIRECTORY NAME in this case to
explicitly tell the server to re-encode the database directory name to
the current encoding format:

ALTER DATABASE `#mysql50#a-b-c` UPGRADE DATA DIRECTORY NAME;

After executing this statement, you can refer to the database as a-b-c
without the special #mysql50# prefix.

URL: http://dev.mysql.com/doc/refman/5.5/en/alter-database.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/alter-database.html</field>
	</row>
	<row>
		<field name="help_topic_id">214</field>
		<field name="name">GEOMETRYN</field>
		<field name="help_category_id">26</field>
		<field name="description">GeometryN(gc,N)

Returns the N-th geometry in the GeometryCollection value gc.
Geometries are numbered beginning with 1.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SET @gc = 'GeometryCollection(Point(1 1),LineString(2 2, 3 3))';
mysql&gt; SELECT AsText(GeometryN(GeomFromText(@gc),1));
+----------------------------------------+
| AsText(GeometryN(GeomFromText(@gc),1)) |
+----------------------------------------+
| POINT(1 1)                             |
+----------------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">215</field>
		<field name="name">&lt;&lt;</field>
		<field name="help_category_id">20</field>
		<field name="description">Syntax:
&lt;&lt;

Shifts a longlong (BIGINT) number to the left.

URL: http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html

</field>
		<field name="example">mysql&gt; SELECT 1 &lt;&lt; 2;
        -&gt; 4
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">216</field>
		<field name="name">SHOW TABLE STATUS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW TABLE STATUS [{FROM | IN} db_name]
    [LIKE 'pattern' | WHERE expr]

SHOW TABLE STATUS works likes SHOW TABLES, but provides a lot of
information about each non-TEMPORARY table. You can also get this list
using the mysqlshow --status db_name command. The LIKE clause, if
present, indicates which table names to match. The WHERE clause can be
given to select rows using more general conditions, as discussed in
http://dev.mysql.com/doc/refman/5.5/en/extended-show.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-table-status.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-table-status.html</field>
	</row>
	<row>
		<field name="help_topic_id">217</field>
		<field name="name">MD5</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
MD5(str)

Calculates an MD5 128-bit checksum for the string. The value is
returned as a string of 32 hex digits, or NULL if the argument was
NULL. The return value can, for example, be used as a hash key. See the
notes at the beginning of this section about storing hash values
efficiently.

As of MySQL 5.5.3, the return value is a nonbinary string in the
connection character set. Before 5.5.3, the return value is a binary
string; see the notes at the beginning of this section about using the
value as a nonbinary string.

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example">mysql&gt; SELECT MD5('testing');
        -&gt; 'ae2b1fca515949e5d54fb22b8ed95575'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">218</field>
		<field name="name">&lt;</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
&lt;

Less than:

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT 2 &lt; 2;
        -&gt; 0
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">219</field>
		<field name="name">UNIX_TIMESTAMP</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
UNIX_TIMESTAMP(), UNIX_TIMESTAMP(date)

If called with no argument, returns a Unix timestamp (seconds since
'1970-01-01 00:00:00' UTC) as an unsigned integer. If UNIX_TIMESTAMP()
is called with a date argument, it returns the value of the argument as
seconds since '1970-01-01 00:00:00' UTC. date may be a DATE string, a
DATETIME string, a TIMESTAMP, or a number in the format YYMMDD or
YYYYMMDD. The server interprets date as a value in the current time
zone and converts it to an internal value in UTC. Clients can set their
time zone as described in
http://dev.mysql.com/doc/refman/5.5/en/time-zone-support.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT UNIX_TIMESTAMP();
        -&gt; 1196440210
mysql&gt; SELECT UNIX_TIMESTAMP('2007-11-30 10:30:19');
        -&gt; 1196440219
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">220</field>
		<field name="name">DAYOFMONTH</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
DAYOFMONTH(date)

Returns the day of the month for date, in the range 1 to 31, or 0 for
dates such as '0000-00-00' or '2008-00-00' that have a zero day part.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT DAYOFMONTH('2007-02-03');
        -&gt; 3
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">221</field>
		<field name="name">ASCII</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
ASCII(str)

Returns the numeric value of the leftmost character of the string str.
Returns 0 if str is the empty string. Returns NULL if str is NULL.
ASCII() works for 8-bit characters.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT ASCII('2');
        -&gt; 50
mysql&gt; SELECT ASCII(2);
        -&gt; 50
mysql&gt; SELECT ASCII('dx');
        -&gt; 100
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">222</field>
		<field name="name">DIV</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
DIV

Integer division. Similar to FLOOR(), but is safe with BIGINT values.

As of MySQL 5.5.3, if either operand has a noninteger type, the
operands are converted to DECIMAL and divided using DECIMAL arithmetic
before converting the result to BIGINT. If the result exceeds BIGINT
range, an error occurs. Before MySQL 5.5.3, incorrect results may occur
for noninteger operands that exceed BIGINT range.

URL: http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html

</field>
		<field name="example">mysql&gt; SELECT 5 DIV 2;
        -&gt; 2
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">223</field>
		<field name="name">RENAME USER</field>
		<field name="help_category_id">10</field>
		<field name="description">Syntax:
RENAME USER old_user TO new_user
    [, old_user TO new_user] ...

The RENAME USER statement renames existing MySQL accounts. To use it,
you must have the global CREATE USER privilege or the UPDATE privilege
for the mysql database. An error occurs if any old account does not
exist or any new account exists. Each account name uses the format
described in http://dev.mysql.com/doc/refman/5.5/en/account-names.html.
For example:

RENAME USER 'jeffrey'@'localhost' TO 'jeff'@'127.0.0.1';

If you specify only the user name part of the account name, a host name
part of '%' is used.

RENAME USER causes the privileges held by the old user to be those held
by the new user. However, RENAME USER does not automatically drop or
invalidate databases or objects within them that the old user created.
This includes stored programs or views for which the DEFINER attribute
names the old user. Attempts to access such objects may produce an
error if they execute in definer security context. (For information
about security context, see
http://dev.mysql.com/doc/refman/5.5/en/stored-programs-security.html.)

The privilege changes take effect as indicated in
http://dev.mysql.com/doc/refman/5.5/en/privilege-changes.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/rename-user.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/rename-user.html</field>
	</row>
	<row>
		<field name="help_topic_id">224</field>
		<field name="name">SHOW SLAVE STATUS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW SLAVE STATUS

This statement provides status information on essential parameters of
the slave threads. It requires either the SUPER or REPLICATION CLIENT
privilege.

If you issue this statement using the mysql client, you can use a \G
statement terminator rather than a semicolon to obtain a more readable
vertical layout:

mysql&gt; SHOW SLAVE STATUS\G
*************************** 1. row ***************************
               Slave_IO_State: Waiting for master to send event
                  Master_Host: localhost
                  Master_User: root
                  Master_Port: 3306
                Connect_Retry: 3
              Master_Log_File: gbichot-bin.005
          Read_Master_Log_Pos: 79
               Relay_Log_File: gbichot-relay-bin.005
                Relay_Log_Pos: 548
        Relay_Master_Log_File: gbichot-bin.005
             Slave_IO_Running: Yes
            Slave_SQL_Running: Yes
              Replicate_Do_DB:
          Replicate_Ignore_DB:
           Replicate_Do_Table:
       Replicate_Ignore_Table:
      Replicate_Wild_Do_Table:
  Replicate_Wild_Ignore_Table:
                   Last_Errno: 0
                   Last_Error:
                 Skip_Counter: 0
          Exec_Master_Log_Pos: 79
              Relay_Log_Space: 552
              Until_Condition: None
               Until_Log_File:
                Until_Log_Pos: 0
           Master_SSL_Allowed: No
           Master_SSL_CA_File:
           Master_SSL_CA_Path:
              Master_SSL_Cert:
            Master_SSL_Cipher:
               Master_SSL_Key:
        Seconds_Behind_Master: 8
Master_SSL_Verify_Server_Cert: No
                Last_IO_Errno: 0
                Last_IO_Error:
               Last_SQL_Errno: 0
               Last_SQL_Error:
  Replicate_Ignore_Server_Ids: 0
             Master_Server_Id: 1

URL: http://dev.mysql.com/doc/refman/5.5/en/show-slave-status.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-slave-status.html</field>
	</row>
	<row>
		<field name="help_topic_id">225</field>
		<field name="name">GEOMETRY</field>
		<field name="help_category_id">35</field>
		<field name="description">MySQL provides a standard way of creating spatial columns for geometry
types, for example, with CREATE TABLE or ALTER TABLE. Currently,
spatial columns are supported for MyISAM, InnoDB, NDB, and ARCHIVE
tables. See also the annotations about spatial indexes under [HELP
SPATIAL].

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-columns.html

</field>
		<field name="example">CREATE TABLE geom (g GEOMETRY);
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-columns.html</field>
	</row>
	<row>
		<field name="help_topic_id">226</field>
		<field name="name">NUMPOINTS</field>
		<field name="help_category_id">13</field>
		<field name="description">NumPoints(ls)

Returns the number of Point objects in the LineString value ls.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SET @ls = 'LineString(1 1,2 2,3 3)';
mysql&gt; SELECT NumPoints(GeomFromText(@ls));
+------------------------------+
| NumPoints(GeomFromText(@ls)) |
+------------------------------+
|                            3 |
+------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">227</field>
		<field name="name">ALTER LOGFILE GROUP</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
ALTER LOGFILE GROUP logfile_group
    ADD UNDOFILE 'file_name'
    [INITIAL_SIZE [=] size]
    [WAIT]
    ENGINE [=] engine_name

This statement adds an UNDO file named 'file_name' to an existing log
file group logfile_group. An ALTER LOGFILE GROUP statement has one and
only one ADD UNDOFILE clause. No DROP UNDOFILE clause is currently
supported.

*Note*: All MySQL Cluster Disk Data objects share the same namespace.
This means that each Disk Data object must be uniquely named (and not
merely each Disk Data object of a given type). For example, you cannot
have a tablespace and an undo log file with the same name, or an undo
log file and a data file with the same name.

The optional INITIAL_SIZE parameter sets the UNDO file's initial size
in bytes; if not specified, the initial size default to 128M (128
megabytes). You may optionally follow size with a one-letter
abbreviation for an order of magnitude, similar to those used in
my.cnf. Generally, this is one of the letters M (for megabytes) or G
(for gigabytes).

On 32-bit systems, the maximum supported value for INITIAL_SIZE is 4G.
(Bug #29186)

The minimum permitted value for INITIAL_SIZE is 1M. (Bug #29574)

*Note*: WAIT is parsed but otherwise ignored. This keyword currently
has no effect, and is intended for future expansion.

The ENGINE parameter (required) determines the storage engine which is
used by this log file group, with engine_name being the name of the
storage engine. Currently, the only accepted values for engine_name are
&quot;NDBCLUSTER&quot; and &quot;NDB&quot;. The two values are equivalent.

URL: http://dev.mysql.com/doc/refman/5.5/en/alter-logfile-group.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/alter-logfile-group.html</field>
	</row>
	<row>
		<field name="help_topic_id">228</field>
		<field name="name">&amp;</field>
		<field name="help_category_id">20</field>
		<field name="description">Syntax:
&amp;

Bitwise AND:

URL: http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html

</field>
		<field name="example">mysql&gt; SELECT 29 &amp; 15;
        -&gt; 13
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">229</field>
		<field name="name">LOCALTIMESTAMP</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
LOCALTIMESTAMP, LOCALTIMESTAMP()

LOCALTIMESTAMP and LOCALTIMESTAMP() are synonyms for NOW().

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">230</field>
		<field name="name">ASSIGN-EQUAL</field>
		<field name="help_category_id">15</field>
		<field name="description">Syntax:
=

This operator is used to perform value assignments in two cases,
described in the next two paragraphs.

Within a SET statement, = is treated as an assignment operator that
causes the user variable on the left hand side of the operator to take
on the value to its right. (In other words, when used in a SET
statement, = is treated identically to :=.) The value on the right hand
side may be a literal value, another variable storing a value, or any
legal expression that yields a scalar value, including the result of a
query (provided that this value is a scalar value). You can perform
multiple assignments in the same SET statement.

In the SET clause of an UPDATE statement, = also acts as an assignment
operator; in this case, however, it causes the column named on the left
hand side of the operator to assume the value given to the right,
provided any WHERE conditions that are part of the UPDATE are met. You
can make multiple assignments in the same SET clause of an UPDATE
statement.

In any other context, = is treated as a comparison operator.

URL: http://dev.mysql.com/doc/refman/5.5/en/assignment-operators.html

</field>
		<field name="example">mysql&gt; SELECT @var1, @var2;
        -&gt; NULL, NULL
mysql&gt; SELECT @var1 := 1, @var2;
        -&gt; 1, NULL
mysql&gt; SELECT @var1, @var2;
        -&gt; 1, NULL
mysql&gt; SELECT @var1, @var2 := @var1;
        -&gt; 1, 1
mysql&gt; SELECT @var1, @var2;
        -&gt; 1, 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/assignment-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">231</field>
		<field name="name">CONVERT</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
CONVERT(expr,type), CONVERT(expr USING transcoding_name)

The CONVERT() and CAST() functions take an expression of any type and
produce a result value of a specified type.

The type for the result can be one of the following values:

o BINARY[(N)]

o CHAR[(N)]

o DATE

o DATETIME

o DECIMAL[(M[,D])]

o SIGNED [INTEGER]

o TIME

o UNSIGNED [INTEGER]

BINARY produces a string with the BINARY data type. See
http://dev.mysql.com/doc/refman/5.5/en/binary-varbinary.html for a
description of how this affects comparisons. If the optional length N
is given, BINARY(N) causes the cast to use no more than N bytes of the
argument. Values shorter than N bytes are padded with 0x00 bytes to a
length of N.

CHAR(N) causes the cast to use no more than N characters of the
argument.

CAST() and CONVERT(... USING ...) are standard SQL syntax. The
non-USING form of CONVERT() is ODBC syntax.

CONVERT() with USING is used to convert data between different
character sets. In MySQL, transcoding names are the same as the
corresponding character set names. For example, this statement converts
the string 'abc' in the default character set to the corresponding
string in the utf8 character set:

SELECT CONVERT('abc' USING utf8);

URL: http://dev.mysql.com/doc/refman/5.5/en/cast-functions.html

</field>
		<field name="example">SELECT enum_col FROM tbl_name ORDER BY CAST(enum_col AS CHAR);
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/cast-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">232</field>
		<field name="name">DROP LOGFILE GROUP</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
DROP LOGFILE GROUP logfile_group
    ENGINE [=] engine_name

This statement drops the log file group named logfile_group. The log
file group must already exist or an error results. (For information on
creating log file groups, see [HELP CREATE LOGFILE GROUP].)

*Important*: Before dropping a log file group, you must drop all
tablespaces that use that log file group for UNDO logging.

The required ENGINE clause provides the name of the storage engine used
by the log file group to be dropped. Currently, the only permitted
values for engine_name are NDB and NDBCLUSTER.

DROP LOGFILE GROUP is useful only with Disk Data storage for MySQL
Cluster. See
http://dev.mysql.com/doc/refman/5.5/en/mysql-cluster-disk-data.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-logfile-group.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-logfile-group.html</field>
	</row>
	<row>
		<field name="help_topic_id">233</field>
		<field name="name">ADDDATE</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
ADDDATE(date,INTERVAL expr unit), ADDDATE(expr,days)

When invoked with the INTERVAL form of the second argument, ADDDATE()
is a synonym for DATE_ADD(). The related function SUBDATE() is a
synonym for DATE_SUB(). For information on the INTERVAL unit argument,
see the discussion for DATE_ADD().

mysql&gt; SELECT DATE_ADD('2008-01-02', INTERVAL 31 DAY);
        -&gt; '2008-02-02'
mysql&gt; SELECT ADDDATE('2008-01-02', INTERVAL 31 DAY);
        -&gt; '2008-02-02'

When invoked with the days form of the second argument, MySQL treats it
as an integer number of days to be added to expr.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT ADDDATE('2008-01-02', 31);
        -&gt; '2008-02-02'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">234</field>
		<field name="name">REPEAT LOOP</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
[begin_label:] REPEAT
    statement_list
UNTIL search_condition
END REPEAT [end_label]

The statement list within a REPEAT statement is repeated until the
search_condition expression is true. Thus, a REPEAT always enters the
loop at least once. statement_list consists of one or more statements,
each terminated by a semicolon (;) statement delimiter.

A REPEAT statement can be labeled. For the rules regarding label use,
see [HELP labels].

URL: http://dev.mysql.com/doc/refman/5.5/en/repeat.html

</field>
		<field name="example">mysql&gt; delimiter //

mysql&gt; CREATE PROCEDURE dorepeat(p1 INT)
    -&gt; BEGIN
    -&gt;   SET @x = 0;
    -&gt;   REPEAT
    -&gt;     SET @x = @x + 1;
    -&gt;   UNTIL @x &gt; p1 END REPEAT;
    -&gt; END
    -&gt; //
Query OK, 0 rows affected (0.00 sec)

mysql&gt; CALL dorepeat(1000)//
Query OK, 0 rows affected (0.00 sec)

mysql&gt; SELECT @x//
+------+
| @x   |
+------+
| 1001 |
+------+
1 row in set (0.00 sec)
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/repeat.html</field>
	</row>
	<row>
		<field name="help_topic_id">235</field>
		<field name="name">ALTER FUNCTION</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
ALTER FUNCTION func_name [characteristic ...]

characteristic:
    COMMENT 'string'
  | LANGUAGE SQL
  | { CONTAINS SQL | NO SQL | READS SQL DATA | MODIFIES SQL DATA }
  | SQL SECURITY { DEFINER | INVOKER }

This statement can be used to change the characteristics of a stored
function. More than one change may be specified in an ALTER FUNCTION
statement. However, you cannot change the parameters or body of a
stored function using this statement; to make such changes, you must
drop and re-create the function using DROP FUNCTION and CREATE
FUNCTION.

You must have the ALTER ROUTINE privilege for the function. (That
privilege is granted automatically to the function creator.) If binary
logging is enabled, the ALTER FUNCTION statement might also require the
SUPER privilege, as described in
http://dev.mysql.com/doc/refman/5.5/en/stored-programs-logging.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/alter-function.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/alter-function.html</field>
	</row>
	<row>
		<field name="help_topic_id">236</field>
		<field name="name">SMALLINT</field>
		<field name="help_category_id">23</field>
		<field name="description">SMALLINT[(M)] [UNSIGNED] [ZEROFILL]

A small integer. The signed range is -32768 to 32767. The unsigned
range is 0 to 65535.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">237</field>
		<field name="name">DOUBLE PRECISION</field>
		<field name="help_category_id">23</field>
		<field name="description">DOUBLE PRECISION[(M,D)] [UNSIGNED] [ZEROFILL], REAL[(M,D)] [UNSIGNED]
[ZEROFILL]

These types are synonyms for DOUBLE. Exception: If the REAL_AS_FLOAT
SQL mode is enabled, REAL is a synonym for FLOAT rather than DOUBLE.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">238</field>
		<field name="name">ORD</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
ORD(str)

If the leftmost character of the string str is a multi-byte character,
returns the code for that character, calculated from the numeric values
of its constituent bytes using this formula:

  (1st byte code)
+ (2nd byte code * 256)
+ (3rd byte code * 2562) ...

If the leftmost character is not a multi-byte character, ORD() returns
the same value as the ASCII() function.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT ORD('2');
        -&gt; 50
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">239</field>
		<field name="name">DEALLOCATE PREPARE</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
{DEALLOCATE | DROP} PREPARE stmt_name

To deallocate a prepared statement produced with PREPARE, use a
DEALLOCATE PREPARE statement that refers to the prepared statement
name. Attempting to execute a prepared statement after deallocating it
results in an error.

URL: http://dev.mysql.com/doc/refman/5.5/en/deallocate-prepare.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/deallocate-prepare.html</field>
	</row>
	<row>
		<field name="help_topic_id">240</field>
		<field name="name">ENVELOPE</field>
		<field name="help_category_id">37</field>
		<field name="description">Envelope(g)

Returns the Minimum Bounding Rectangle (MBR) for the geometry value g.
The result is returned as a Polygon value.

The polygon is defined by the corner points of the bounding box:

POLYGON((MINX MINY, MAXX MINY, MAXX MAXY, MINX MAXY, MINX MINY))

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SELECT AsText(Envelope(GeomFromText('LineString(1 1,2 2)')));
+-------------------------------------------------------+
| AsText(Envelope(GeomFromText('LineString(1 1,2 2)'))) |
+-------------------------------------------------------+
| POLYGON((1 1,2 1,2 2,1 2,1 1))                        |
+-------------------------------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">241</field>
		<field name="name">IS_FREE_LOCK</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
IS_FREE_LOCK(str)

Checks whether the lock named str is free to use (that is, not locked).
Returns 1 if the lock is free (no one is using the lock), 0 if the lock
is in use, and NULL if an error occurs (such as an incorrect argument).

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">242</field>
		<field name="name">TOUCHES</field>
		<field name="help_category_id">31</field>
		<field name="description">Touches(g1,g2)

Returns 1 or 0 to indicate whether g1 spatially touches g2. Two
geometries spatially touch if the interiors of the geometries do not
intersect, but the boundary of one of the geometries intersects either
the boundary or the interior of the other.

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">243</field>
		<field name="name">INET_ATON</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
INET_ATON(expr)

Given the dotted-quad representation of an IPv4 network address as a
string, returns an integer that represents the numeric value of the
address in network byte order (big endian). INET_ATON() returns NULL if
it does not understand its argument.

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example">mysql&gt; SELECT INET_ATON('10.0.5.9');
        -&gt; 167773449
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">244</field>
		<field name="name">UNCOMPRESS</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
UNCOMPRESS(string_to_uncompress)

Uncompresses a string compressed by the COMPRESS() function. If the
argument is not a compressed value, the result is NULL. This function
requires MySQL to have been compiled with a compression library such as
zlib. Otherwise, the return value is always NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example">mysql&gt; SELECT UNCOMPRESS(COMPRESS('any string'));
        -&gt; 'any string'
mysql&gt; SELECT UNCOMPRESS('any string');
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">245</field>
		<field name="name">AUTO_INCREMENT</field>
		<field name="help_category_id">23</field>
		<field name="description">The AUTO_INCREMENT attribute can be used to generate a unique identity
for new rows:

URL: http://dev.mysql.com/doc/refman/5.5/en/example-auto-increment.html

</field>
		<field name="example">CREATE TABLE animals (
     id MEDIUMINT NOT NULL AUTO_INCREMENT,
     name CHAR(30) NOT NULL,
     PRIMARY KEY (id)
);

INSERT INTO animals (name) VALUES
    ('dog'),('cat'),('penguin'),
    ('lax'),('whale'),('ostrich');

SELECT * FROM animals;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/example-auto-increment.html</field>
	</row>
	<row>
		<field name="help_topic_id">246</field>
		<field name="name">ISSIMPLE</field>
		<field name="help_category_id">37</field>
		<field name="description">IsSimple(g)

In MySQL 5.5, this function is a placeholder that always returns 0.

The description of each instantiable geometric class given earlier in
the chapter includes the specific conditions that cause an instance of
that class to be classified as not simple. (See [HELP Geometry
hierarchy].)

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">247</field>
		<field name="name">- BINARY</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
-

Subtraction:

URL: http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html

</field>
		<field name="example">mysql&gt; SELECT 3-5;
        -&gt; -2
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">248</field>
		<field name="name">GEOMCOLLFROMTEXT</field>
		<field name="help_category_id">3</field>
		<field name="description">GeomCollFromText(wkt[,srid]), GeometryCollectionFromText(wkt[,srid])

Constructs a GEOMETRYCOLLECTION value using its WKT representation and
SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">249</field>
		<field name="name">WKT DEFINITION</field>
		<field name="help_category_id">3</field>
		<field name="description">The Well-Known Text (WKT) representation of Geometry is designed to
exchange geometry data in ASCII form. For a Backus-Naur grammar that
specifies the formal production rules for writing WKT values, see the
OpenGIS specification document referenced in
http://dev.mysql.com/doc/refman/5.5/en/spatial-extensions.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/gis-wkt-format.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/gis-wkt-format.html</field>
	</row>
	<row>
		<field name="help_topic_id">250</field>
		<field name="name">CURRENT_TIME</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
CURRENT_TIME, CURRENT_TIME()

CURRENT_TIME and CURRENT_TIME() are synonyms for CURTIME().

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">251</field>
		<field name="name">REVOKE</field>
		<field name="help_category_id">10</field>
		<field name="description">Syntax:
REVOKE
    priv_type [(column_list)]
      [, priv_type [(column_list)]] ...
    ON [object_type] priv_level
    FROM user [, user] ...

REVOKE ALL PRIVILEGES, GRANT OPTION
    FROM user [, user] ...

REVOKE PROXY ON user
    FROM user [, user] ...

The REVOKE statement enables system administrators to revoke privileges
from MySQL accounts. Each account name uses the format described in
http://dev.mysql.com/doc/refman/5.5/en/account-names.html. For example:

REVOKE INSERT ON *.* FROM 'jeffrey'@'localhost';

If you specify only the user name part of the account name, a host name
part of '%' is used.

For details on the levels at which privileges exist, the permissible
priv_type and priv_level values, and the syntax for specifying users
and passwords, see [HELP GRANT]

To use the first REVOKE syntax, you must have the GRANT OPTION
privilege, and you must have the privileges that you are revoking.

To revoke all privileges, use the second syntax, which drops all
global, database, table, column, and routine privileges for the named
user or users:

REVOKE ALL PRIVILEGES, GRANT OPTION FROM user [, user] ...

To use this REVOKE syntax, you must have the global CREATE USER
privilege or the UPDATE privilege for the mysql database.

URL: http://dev.mysql.com/doc/refman/5.5/en/revoke.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/revoke.html</field>
	</row>
	<row>
		<field name="help_topic_id">252</field>
		<field name="name">LAST_INSERT_ID</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
LAST_INSERT_ID(), LAST_INSERT_ID(expr)

LAST_INSERT_ID() (with no argument) returns a BIGINT (64-bit) value
representing the first automatically generated value successfully
inserted for an AUTO_INCREMENT column as a result of the most recently
executed INSERT statement. The value of LAST_INSERT_ID() remains
unchanged if no rows are successfully inserted.

For example, after inserting a row that generates an AUTO_INCREMENT
value, you can get the value like this:

mysql&gt; SELECT LAST_INSERT_ID();
        -&gt; 195

The currently executing statement does not affect the value of
LAST_INSERT_ID(). Suppose that you generate an AUTO_INCREMENT value
with one statement, and then refer to LAST_INSERT_ID() in a
multiple-row INSERT statement that inserts rows into a table with its
own AUTO_INCREMENT column. The value of LAST_INSERT_ID() will remain
stable in the second statement; its value for the second and later rows
is not affected by the earlier row insertions. (However, if you mix
references to LAST_INSERT_ID() and LAST_INSERT_ID(expr), the effect is
undefined.)

If the previous statement returned an error, the value of
LAST_INSERT_ID() is undefined. For transactional tables, if the
statement is rolled back due to an error, the value of LAST_INSERT_ID()
is left undefined. For manual ROLLBACK, the value of LAST_INSERT_ID()
is not restored to that before the transaction; it remains as it was at
the point of the ROLLBACK.

Within the body of a stored routine (procedure or function) or a
trigger, the value of LAST_INSERT_ID() changes the same way as for
statements executed outside the body of these kinds of objects. The
effect of a stored routine or trigger upon the value of
LAST_INSERT_ID() that is seen by following statements depends on the
kind of routine:

o If a stored procedure executes statements that change the value of
  LAST_INSERT_ID(), the changed value is seen by statements that follow
  the procedure call.

o For stored functions and triggers that change the value, the value is
  restored when the function or trigger ends, so following statements
  will not see a changed value.

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">253</field>
		<field name="name">LAST_DAY</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
LAST_DAY(date)

Takes a date or datetime value and returns the corresponding value for
the last day of the month. Returns NULL if the argument is invalid.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT LAST_DAY('2003-02-05');
        -&gt; '2003-02-28'
mysql&gt; SELECT LAST_DAY('2004-02-05');
        -&gt; '2004-02-29'
mysql&gt; SELECT LAST_DAY('2004-01-01 01:01:01');
        -&gt; '2004-01-31'
mysql&gt; SELECT LAST_DAY('2003-03-32');
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">254</field>
		<field name="name">MEDIUMINT</field>
		<field name="help_category_id">23</field>
		<field name="description">MEDIUMINT[(M)] [UNSIGNED] [ZEROFILL]

A medium-sized integer. The signed range is -8388608 to 8388607. The
unsigned range is 0 to 16777215.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">255</field>
		<field name="name">FLOOR</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
FLOOR(X)

Returns the largest integer value not greater than X.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT FLOOR(1.23);
        -&gt; 1
mysql&gt; SELECT FLOOR(-1.23);
        -&gt; -2
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">256</field>
		<field name="name">RTRIM</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
RTRIM(str)

Returns the string str with trailing space characters removed.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT RTRIM('barbar   ');
        -&gt; 'barbar'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">257</field>
		<field name="name">EXPLAIN</field>
		<field name="help_category_id">29</field>
		<field name="description">Syntax:
{EXPLAIN | DESCRIBE | DESC}
    tbl_name [col_name | wild]

{EXPLAIN | DESCRIBE | DESC}
    [explain_type] SELECT select_options

explain_type:
    EXTENDED
  | PARTITIONS

The DESCRIBE and EXPLAIN statements are synonyms. In practice, the
DESCRIBE keyword is more often used to obtain information about table
structure, whereas EXPLAIN is used to obtain a query execution plan
(that is, an explanation of how MySQL would execute a query). The
following discussion uses the DESCRIBE and EXPLAIN keywords in
accordance with those uses, but the MySQL parser treats them as
completely synonymous.

Obtaining Table Structure Information

DESCRIBE provides information about the columns in a table. It is a
shortcut for SHOW COLUMNS FROM. These statements also display
information for views. (See [HELP SHOW COLUMNS].)

col_name can be a column name, or a string containing the SQL &quot;%&quot; and
&quot;_&quot; wildcard characters to obtain output only for the columns with
names matching the string. There is no need to enclose the string
within quotation marks unless it contains spaces or other special
characters.

mysql&gt; DESCRIBE City;
+------------+----------+------+-----+---------+----------------+
| Field      | Type     | Null | Key | Default | Extra          |
+------------+----------+------+-----+---------+----------------+
| Id         | int(11)  | NO   | PRI | NULL    | auto_increment |
| Name       | char(35) | NO   |     |         |                |
| Country    | char(3)  | NO   | UNI |         |                |
| District   | char(20) | YES  | MUL |         |                |
| Population | int(11)  | NO   |     | 0       |                |
+------------+----------+------+-----+---------+----------------+
5 rows in set (0.00 sec)

The description for SHOW COLUMNS provides more information about the
output columns (see [HELP SHOW COLUMNS]).

If the data types differ from what you expect them to be based on a
CREATE TABLE statement, note that MySQL sometimes changes data types
when you create or alter a table. The conditions under which this
occurs are described in
http://dev.mysql.com/doc/refman/5.5/en/silent-column-changes.html.

The DESCRIBE statement is provided for compatibility with Oracle.

The SHOW CREATE TABLE, SHOW TABLE STATUS, and SHOW INDEX statements
also provide information about tables. See [HELP SHOW].

Obtaining Query Execution Plan Information

The EXPLAIN statement provides a way to obtain information about how
MySQL executes a statement:

o When you precede a SELECT statement with the keyword EXPLAIN, MySQL
  displays information from the optimizer about the query execution
  plan. That is, MySQL explains how it would process the statement,
  including information about how tables are joined and in which order.
  EXPLAIN EXTENDED can be used to obtain additional information.

  For information about using EXPLAIN and EXPLAIN EXTENDED to obtain
  query execution plan information, see
  http://dev.mysql.com/doc/refman/5.5/en/using-explain.html.

o EXPLAIN PARTITIONS is useful only when examining queries involving
  partitioned tables. For details, see
  http://dev.mysql.com/doc/refman/5.5/en/partitioning-info.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/explain.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/explain.html</field>
	</row>
	<row>
		<field name="help_topic_id">258</field>
		<field name="name">DEGREES</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
DEGREES(X)

Returns the argument X, converted from radians to degrees.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT DEGREES(PI());
        -&gt; 180
mysql&gt; SELECT DEGREES(PI() / 2);
        -&gt; 90
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">259</field>
		<field name="name">VARCHAR</field>
		<field name="help_category_id">23</field>
		<field name="description">[NATIONAL] VARCHAR(M) [CHARACTER SET charset_name] [COLLATE
collation_name]

A variable-length string. M represents the maximum column length in
characters. The range of M is 0 to 65,535. The effective maximum length
of a VARCHAR is subject to the maximum row size (65,535 bytes, which is
shared among all columns) and the character set used. For example, utf8
characters can require up to three bytes per character, so a VARCHAR
column that uses the utf8 character set can be declared to be a maximum
of 21,844 characters. See
http://dev.mysql.com/doc/refman/5.5/en/column-count-limit.html.

MySQL stores VARCHAR values as a 1-byte or 2-byte length prefix plus
data. The length prefix indicates the number of bytes in the value. A
VARCHAR column uses one length byte if values require no more than 255
bytes, two length bytes if values may require more than 255 bytes.

*Note*: MySQL 5.5 follows the standard SQL specification, and does not
remove trailing spaces from VARCHAR values.

VARCHAR is shorthand for CHARACTER VARYING. NATIONAL VARCHAR is the
standard SQL way to define that a VARCHAR column should use some
predefined character set. MySQL 4.1 and up uses utf8 as this predefined
character set.
http://dev.mysql.com/doc/refman/5.5/en/charset-national.html. NVARCHAR
is shorthand for NATIONAL VARCHAR.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">260</field>
		<field name="name">UNHEX</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:

UNHEX(str)

For a string argument str, UNHEX(str) performs the inverse operation of
HEX(str). That is, it interprets each pair of characters in the
argument as a hexadecimal number and converts it to the character
represented by the number. The return value is a binary string.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT UNHEX('4D7953514C');
        -&gt; 'MySQL'
mysql&gt; SELECT 0x4D7953514C;
        -&gt; 'MySQL'
mysql&gt; SELECT UNHEX(HEX('string'));
        -&gt; 'string'
mysql&gt; SELECT HEX(UNHEX('1267'));
        -&gt; '1267'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">261</field>
		<field name="name">- UNARY</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
-

Unary minus. This operator changes the sign of the operand.

URL: http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html

</field>
		<field name="example">mysql&gt; SELECT - 2;
        -&gt; -2
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">262</field>
		<field name="name">STD</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
STD(expr)

Returns the population standard deviation of expr. This is an extension
to standard SQL. The standard SQL function STDDEV_POP() can be used
instead.

This function returns NULL if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">263</field>
		<field name="name">COS</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
COS(X)

Returns the cosine of X, where X is given in radians.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT COS(PI());
        -&gt; -1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">264</field>
		<field name="name">DATE FUNCTION</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
DATE(expr)

Extracts the date part of the date or datetime expression expr.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT DATE('2003-12-31 01:02:03');
        -&gt; '2003-12-31'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">265</field>
		<field name="name">DROP TRIGGER</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
DROP TRIGGER [IF EXISTS] [schema_name.]trigger_name

This statement drops a trigger. The schema (database) name is optional.
If the schema is omitted, the trigger is dropped from the default
schema. DROP TRIGGER requires the TRIGGER privilege for the table
associated with the trigger.

Use IF EXISTS to prevent an error from occurring for a trigger that
does not exist. A NOTE is generated for a nonexistent trigger when
using IF EXISTS. See [HELP SHOW WARNINGS].

Triggers for a table are also dropped if you drop the table.

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-trigger.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-trigger.html</field>
	</row>
	<row>
		<field name="help_topic_id">266</field>
		<field name="name">RESET MASTER</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
RESET MASTER

Deletes all binary log files listed in the index file, resets the
binary log index file to be empty, and creates a new binary log file.
This statement is intended to be used only when the master is started
for the first time.

URL: http://dev.mysql.com/doc/refman/5.5/en/reset-master.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/reset-master.html</field>
	</row>
	<row>
		<field name="help_topic_id">267</field>
		<field name="name">TAN</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
TAN(X)

Returns the tangent of X, where X is given in radians.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT TAN(PI());
        -&gt; -1.2246063538224e-16
mysql&gt; SELECT TAN(PI()+1);
        -&gt; 1.5574077246549
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">268</field>
		<field name="name">PI</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
PI()

Returns the value of π (pi). The default number of decimal places
displayed is seven, but MySQL uses the full double-precision value
internally.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT PI();
        -&gt; 3.141593
mysql&gt; SELECT PI()+0.000000000000000000;
        -&gt; 3.141592653589793116
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">269</field>
		<field name="name">WEEKOFYEAR</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
WEEKOFYEAR(date)

Returns the calendar week of the date as a number in the range from 1
to 53. WEEKOFYEAR() is a compatibility function that is equivalent to
WEEK(date,3).

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT WEEKOFYEAR('2008-02-20');
        -&gt; 8
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">270</field>
		<field name="name">/</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
/

Division:

URL: http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html

</field>
		<field name="example">mysql&gt; SELECT 3/5;
        -&gt; 0.60
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">271</field>
		<field name="name">PURGE BINARY LOGS</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
PURGE { BINARY | MASTER } LOGS
    { TO 'log_name' | BEFORE datetime_expr }

The binary log is a set of files that contain information about data
modifications made by the MySQL server. The log consists of a set of
binary log files, plus an index file (see
http://dev.mysql.com/doc/refman/5.5/en/binary-log.html).

The PURGE BINARY LOGS statement deletes all the binary log files listed
in the log index file prior to the specified log file name or date.
BINARY and MASTER are synonyms. Deleted log files also are removed from
the list recorded in the index file, so that the given log file becomes
the first in the list.

This statement has no effect if the server was not started with the
--log-bin option to enable binary logging.

URL: http://dev.mysql.com/doc/refman/5.5/en/purge-binary-logs.html

</field>
		<field name="example">PURGE BINARY LOGS TO 'mysql-bin.010';
PURGE BINARY LOGS BEFORE '2008-04-02 22:46:26';
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/purge-binary-logs.html</field>
	</row>
	<row>
		<field name="help_topic_id">272</field>
		<field name="name">STDDEV_SAMP</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
STDDEV_SAMP(expr)

Returns the sample standard deviation of expr (the square root of
VAR_SAMP().

STDDEV_SAMP() returns NULL if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">273</field>
		<field name="name">SCHEMA</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
SCHEMA()

This function is a synonym for DATABASE().

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">274</field>
		<field name="name">MLINEFROMWKB</field>
		<field name="help_category_id">33</field>
		<field name="description">MLineFromWKB(wkb[,srid]), MultiLineStringFromWKB(wkb[,srid])

Constructs a MULTILINESTRING value using its WKB representation and
SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">275</field>
		<field name="name">LOG2</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
LOG2(X)

Returns the base-2 logarithm of X.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT LOG2(65536);
        -&gt; 16
mysql&gt; SELECT LOG2(-100);
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">276</field>
		<field name="name">SUBTIME</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
SUBTIME(expr1,expr2)

SUBTIME() returns expr1 - expr2 expressed as a value in the same format
as expr1. expr1 is a time or datetime expression, and expr2 is a time
expression.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT SUBTIME('2007-12-31 23:59:59.999999','1 1:1:1.000002');
        -&gt; '2007-12-30 22:58:58.999997'
mysql&gt; SELECT SUBTIME('01:00:00.999999', '02:00:00.999998');
        -&gt; '-00:59:59.999999'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">277</field>
		<field name="name">UNCOMPRESSED_LENGTH</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
UNCOMPRESSED_LENGTH(compressed_string)

Returns the length that the compressed string had before being
compressed.

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example">mysql&gt; SELECT UNCOMPRESSED_LENGTH(COMPRESS(REPEAT('a',30)));
        -&gt; 30
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">278</field>
		<field name="name">DROP TABLE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
DROP [TEMPORARY] TABLE [IF EXISTS]
    tbl_name [, tbl_name] ...
    [RESTRICT | CASCADE]

DROP TABLE removes one or more tables. You must have the DROP privilege
for each table. All table data and the table definition are removed, so
be careful with this statement! If any of the tables named in the
argument list do not exist, MySQL returns an error indicating by name
which nonexisting tables it was unable to drop, but it also drops all
of the tables in the list that do exist.

*Important*: When a table is dropped, user privileges on the table are
not automatically dropped. See [HELP GRANT].

Note that for a partitioned table, DROP TABLE permanently removes the
table definition, all of its partitions, and all of the data which was
stored in those partitions. It also removes the partitioning definition
(.par) file associated with the dropped table.

Use IF EXISTS to prevent an error from occurring for tables that do not
exist. A NOTE is generated for each nonexistent table when using IF
EXISTS. See [HELP SHOW WARNINGS].

RESTRICT and CASCADE are permitted to make porting easier. In MySQL
5.5, they do nothing.

*Note*: DROP TABLE automatically commits the current active
transaction, unless you use the TEMPORARY keyword.

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-table.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-table.html</field>
	</row>
	<row>
		<field name="help_topic_id">279</field>
		<field name="name">POW</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
POW(X,Y)

Returns the value of X raised to the power of Y.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT POW(2,2);
        -&gt; 4
mysql&gt; SELECT POW(2,-2);
        -&gt; 0.25
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">280</field>
		<field name="name">SHOW CREATE TABLE</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW CREATE TABLE tbl_name

Shows the CREATE TABLE statement that creates the given table. To use
this statement, you must have some privilege for the table. This
statement also works with views.
SHOW CREATE TABLE quotes table and column names according to the value
of the sql_quote_show_create option. See
http://dev.mysql.com/doc/refman/5.5/en/server-system-variables.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-create-table.html

</field>
		<field name="example">mysql&gt; SHOW CREATE TABLE t\G
*************************** 1. row ***************************
       Table: t
Create Table: CREATE TABLE t (
  id INT(11) default NULL auto_increment,
  s char(60) default NULL,
  PRIMARY KEY (id)
) ENGINE=MyISAM
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-create-table.html</field>
	</row>
	<row>
		<field name="help_topic_id">281</field>
		<field name="name">DUAL</field>
		<field name="help_category_id">28</field>
		<field name="description">You are permitted to specify DUAL as a dummy table name in situations
where no tables are referenced:

mysql&gt; SELECT 1 + 1 FROM DUAL;
        -&gt; 2

DUAL is purely for the convenience of people who require that all
SELECT statements should have FROM and possibly other clauses. MySQL
may ignore the clauses. MySQL does not require FROM DUAL if no tables
are referenced.

URL: http://dev.mysql.com/doc/refman/5.5/en/select.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/select.html</field>
	</row>
	<row>
		<field name="help_topic_id">282</field>
		<field name="name">INSTR</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
INSTR(str,substr)

Returns the position of the first occurrence of substring substr in
string str. This is the same as the two-argument form of LOCATE(),
except that the order of the arguments is reversed.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT INSTR('foobarbar', 'bar');
        -&gt; 4
mysql&gt; SELECT INSTR('xbar', 'foobar');
        -&gt; 0
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">283</field>
		<field name="name">NOW</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
NOW()

Returns the current date and time as a value in 'YYYY-MM-DD HH:MM:SS'
or YYYYMMDDHHMMSS.uuuuuu format, depending on whether the function is
used in a string or numeric context. The value is expressed in the
current time zone.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT NOW();
        -&gt; '2007-12-15 23:50:26'
mysql&gt; SELECT NOW() + 0;
        -&gt; 20071215235026.000000
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">284</field>
		<field name="name">SHOW ENGINES</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW [STORAGE] ENGINES

SHOW ENGINES displays status information about the server's storage
engines. This is particularly useful for checking whether a storage
engine is supported, or to see what the default engine is.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-engines.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-engines.html</field>
	</row>
	<row>
		<field name="help_topic_id">285</field>
		<field name="name">&gt;=</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
&gt;=

Greater than or equal:

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT 2 &gt;= 2;
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">286</field>
		<field name="name">EXP</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
EXP(X)

Returns the value of e (the base of natural logarithms) raised to the
power of X. The inverse of this function is LOG() (using a single
argument only) or LN().

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT EXP(2);
        -&gt; 7.3890560989307
mysql&gt; SELECT EXP(-2);
        -&gt; 0.13533528323661
mysql&gt; SELECT EXP(0);
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">287</field>
		<field name="name">LONGBLOB</field>
		<field name="help_category_id">23</field>
		<field name="description">LONGBLOB

A BLOB column with a maximum length of 4,294,967,295 or 4GB (232 - 1)
bytes. The effective maximum length of LONGBLOB columns depends on the
configured maximum packet size in the client/server protocol and
available memory. Each LONGBLOB value is stored using a 4-byte length
prefix that indicates the number of bytes in the value.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">288</field>
		<field name="name">POINTN</field>
		<field name="help_category_id">13</field>
		<field name="description">PointN(ls,N)

Returns the N-th Point in the Linestring value ls. Points are numbered
beginning with 1.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SET @ls = 'LineString(1 1,2 2,3 3)';
mysql&gt; SELECT AsText(PointN(GeomFromText(@ls),2));
+-------------------------------------+
| AsText(PointN(GeomFromText(@ls),2)) |
+-------------------------------------+
| POINT(2 2)                          |
+-------------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">289</field>
		<field name="name">YEAR DATA TYPE</field>
		<field name="help_category_id">23</field>
		<field name="description">YEAR[(2|4)]

A year in two-digit or four-digit format. The default is four-digit
format. YEAR(2) or YEAR(4) differ in display format, but have the same
range of values. In four-digit format, values display as 1901 to 2155,
and 0000. In two-digit format, values display as 70 to 69, representing
years from 1970 to 2069. MySQL displays YEAR values in YYYY or
YYformat, but permits assignment of values to YEAR columns using either
strings or numbers.

*Note*: The YEAR(2) data type has certain issues that you should
consider before choosing to use it. As of MySQL 5.5.27, YEAR(2) is
deprecated. For more information, see
http://dev.mysql.com/doc/refman/5.5/en/migrating-to-year4.html.

For additional information about YEAR display format and interpretation
of input values, see http://dev.mysql.com/doc/refman/5.5/en/year.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">290</field>
		<field name="name">SUM</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
SUM([DISTINCT] expr)

Returns the sum of expr. If the return set has no rows, SUM() returns
NULL. The DISTINCT keyword can be used to sum only the distinct values
of expr.

SUM() returns NULL if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">291</field>
		<field name="name">OCT</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
OCT(N)

Returns a string representation of the octal value of N, where N is a
longlong (BIGINT) number. This is equivalent to CONV(N,10,8). Returns
NULL if N is NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT OCT(12);
        -&gt; '14'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">292</field>
		<field name="name">SYSDATE</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
SYSDATE()

Returns the current date and time as a value in 'YYYY-MM-DD HH:MM:SS'
or YYYYMMDDHHMMSS.uuuuuu format, depending on whether the function is
used in a string or numeric context.

SYSDATE() returns the time at which it executes. This differs from the
behavior for NOW(), which returns a constant time that indicates the
time at which the statement began to execute. (Within a stored function
or trigger, NOW() returns the time at which the function or triggering
statement began to execute.)

mysql&gt; SELECT NOW(), SLEEP(2), NOW();
+---------------------+----------+---------------------+
| NOW()               | SLEEP(2) | NOW()               |
+---------------------+----------+---------------------+
| 2006-04-12 13:47:36 |        0 | 2006-04-12 13:47:36 |
+---------------------+----------+---------------------+

mysql&gt; SELECT SYSDATE(), SLEEP(2), SYSDATE();
+---------------------+----------+---------------------+
| SYSDATE()           | SLEEP(2) | SYSDATE()           |
+---------------------+----------+---------------------+
| 2006-04-12 13:47:44 |        0 | 2006-04-12 13:47:46 |
+---------------------+----------+---------------------+

In addition, the SET TIMESTAMP statement affects the value returned by
NOW() but not by SYSDATE(). This means that timestamp settings in the
binary log have no effect on invocations of SYSDATE().

Because SYSDATE() can return different values even within the same
statement, and is not affected by SET TIMESTAMP, it is nondeterministic
and therefore unsafe for replication if statement-based binary logging
is used. If that is a problem, you can use row-based logging.

Alternatively, you can use the --sysdate-is-now option to cause
SYSDATE() to be an alias for NOW(). This works if the option is used on
both the master and the slave.

The nondeterministic nature of SYSDATE() also means that indexes cannot
be used for evaluating expressions that refer to it.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">293</field>
		<field name="name">UNINSTALL PLUGIN</field>
		<field name="help_category_id">5</field>
		<field name="description">Syntax:
UNINSTALL PLUGIN plugin_name

This statement removes an installed server plugin. It requires the
DELETE privilege for the mysql.plugin table.

plugin_name must be the name of some plugin that is listed in the
mysql.plugin table. The server executes the plugin's deinitialization
function and removes the row for the plugin from the mysql.plugin
table, so that subsequent server restarts will not load and initialize
the plugin. UNINSTALL PLUGIN does not remove the plugin's shared
library file.

URL: http://dev.mysql.com/doc/refman/5.5/en/uninstall-plugin.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/uninstall-plugin.html</field>
	</row>
	<row>
		<field name="help_topic_id">294</field>
		<field name="name">ASBINARY</field>
		<field name="help_category_id">33</field>
		<field name="description">AsBinary(g), AsWKB(g)

Converts a value in internal geometry format to its WKB representation
and returns the binary result.

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-to-convert-geometries-between-formats.html

</field>
		<field name="example">SELECT AsBinary(g) FROM geom;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-to-convert-geometries-between-formats.html</field>
	</row>
	<row>
		<field name="help_topic_id">295</field>
		<field name="name">REPEAT FUNCTION</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
REPEAT(str,count)

Returns a string consisting of the string str repeated count times. If
count is less than 1, returns an empty string. Returns NULL if str or
count are NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT REPEAT('MySQL', 3);
        -&gt; 'MySQLMySQLMySQL'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">296</field>
		<field name="name">SHOW TABLES</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW [FULL] TABLES [{FROM | IN} db_name]
    [LIKE 'pattern' | WHERE expr]

SHOW TABLES lists the non-TEMPORARY tables in a given database. You can
also get this list using the mysqlshow db_name command. The LIKE
clause, if present, indicates which table names to match. The WHERE
clause can be given to select rows using more general conditions, as
discussed in http://dev.mysql.com/doc/refman/5.5/en/extended-show.html.

Matching performed by the LIKE clause is dependent on the setting of
the lower_case_table_names system variable.

This statement also lists any views in the database. The FULL modifier
is supported such that SHOW FULL TABLES displays a second output
column. Values for the second column are BASE TABLE for a table and
VIEW for a view.

If you have no privileges for a base table or view, it does not show up
in the output from SHOW TABLES or mysqlshow db_name.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-tables.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-tables.html</field>
	</row>
	<row>
		<field name="help_topic_id">297</field>
		<field name="name">MAKEDATE</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
MAKEDATE(year,dayofyear)

Returns a date, given year and day-of-year values. dayofyear must be
greater than 0 or the result is NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT MAKEDATE(2011,31), MAKEDATE(2011,32);
        -&gt; '2011-01-31', '2011-02-01'
mysql&gt; SELECT MAKEDATE(2011,365), MAKEDATE(2014,365);
        -&gt; '2011-12-31', '2014-12-31'
mysql&gt; SELECT MAKEDATE(2011,0);
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">298</field>
		<field name="name">BINARY OPERATOR</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
BINARY

The BINARY operator casts the string following it to a binary string.
This is an easy way to force a column comparison to be done byte by
byte rather than character by character. This causes the comparison to
be case sensitive even if the column is not defined as BINARY or BLOB.
BINARY also causes trailing spaces to be significant.

URL: http://dev.mysql.com/doc/refman/5.5/en/cast-functions.html

</field>
		<field name="example">mysql&gt; SELECT 'a' = 'A';
        -&gt; 1
mysql&gt; SELECT BINARY 'a' = 'A';
        -&gt; 0
mysql&gt; SELECT 'a' = 'a ';
        -&gt; 1
mysql&gt; SELECT BINARY 'a' = 'a ';
        -&gt; 0
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/cast-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">299</field>
		<field name="name">MBROVERLAPS</field>
		<field name="help_category_id">6</field>
		<field name="description">MBROverlaps(g1,g2)

Returns 1 or 0 to indicate whether the Minimum Bounding Rectangles of
the two geometries g1 and g2 overlap. The term spatially overlaps is
used if two geometries intersect and their intersection results in a
geometry of the same dimension but not equal to either of the given
geometries.

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">300</field>
		<field name="name">SOUNDEX</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
SOUNDEX(str)

Returns a soundex string from str. Two strings that sound almost the
same should have identical soundex strings. A standard soundex string
is four characters long, but the SOUNDEX() function returns an
arbitrarily long string. You can use SUBSTRING() on the result to get a
standard soundex string. All nonalphabetic characters in str are
ignored. All international alphabetic characters outside the A-Z range
are treated as vowels.

*Important*: When using SOUNDEX(), you should be aware of the following
limitations:

o This function, as currently implemented, is intended to work well
  with strings that are in the English language only. Strings in other
  languages may not produce reliable results.

o This function is not guaranteed to provide consistent results with
  strings that use multi-byte character sets, including utf-8.

  We hope to remove these limitations in a future release. See Bug
  #22638 for more information.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT SOUNDEX('Hello');
        -&gt; 'H400'
mysql&gt; SELECT SOUNDEX('Quadratically');
        -&gt; 'Q36324'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">301</field>
		<field name="name">MBRTOUCHES</field>
		<field name="help_category_id">6</field>
		<field name="description">MBRTouches(g1,g2)

Returns 1 or 0 to indicate whether the Minimum Bounding Rectangles of
the two geometries g1 and g2 touch. Two geometries spatially touch if
the interiors of the geometries do not intersect, but the boundary of
one of the geometries intersects either the boundary or the interior of
the other.

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">302</field>
		<field name="name">DROP EVENT</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
DROP EVENT [IF EXISTS] event_name

This statement drops the event named event_name. The event immediately
ceases being active, and is deleted completely from the server.

If the event does not exist, the error ERROR 1517 (HY000): Unknown
event 'event_name' results. You can override this and cause the
statement to generate a warning for nonexistent events instead using IF
EXISTS.

This statement requires the EVENT privilege for the schema to which the
event to be dropped belongs.

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-event.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-event.html</field>
	</row>
	<row>
		<field name="help_topic_id">303</field>
		<field name="name">INSERT SELECT</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
INSERT [LOW_PRIORITY | HIGH_PRIORITY] [IGNORE]
    [INTO] tbl_name [(col_name,...)]
    SELECT ...
    [ ON DUPLICATE KEY UPDATE col_name=expr, ... ]

With INSERT ... SELECT, you can quickly insert many rows into a table
from one or many tables. For example:

INSERT INTO tbl_temp2 (fld_id)
  SELECT tbl_temp1.fld_order_id
  FROM tbl_temp1 WHERE tbl_temp1.fld_order_id &gt; 100;

URL: http://dev.mysql.com/doc/refman/5.5/en/insert-select.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/insert-select.html</field>
	</row>
	<row>
		<field name="help_topic_id">304</field>
		<field name="name">CREATE PROCEDURE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
CREATE
    [DEFINER = { user | CURRENT_USER }]
    PROCEDURE sp_name ([proc_parameter[,...]])
    [characteristic ...] routine_body

CREATE
    [DEFINER = { user | CURRENT_USER }]
    FUNCTION sp_name ([func_parameter[,...]])
    RETURNS type
    [characteristic ...] routine_body

proc_parameter:
    [ IN | OUT | INOUT ] param_name type

func_parameter:
    param_name type

type:
    Any valid MySQL data type

characteristic:
    COMMENT 'string'
  | LANGUAGE SQL
  | [NOT] DETERMINISTIC
  | { CONTAINS SQL | NO SQL | READS SQL DATA | MODIFIES SQL DATA }
  | SQL SECURITY { DEFINER | INVOKER }

routine_body:
    Valid SQL routine statement

These statements create stored routines. By default, a routine is
associated with the default database. To associate the routine
explicitly with a given database, specify the name as db_name.sp_name
when you create it.

The CREATE FUNCTION statement is also used in MySQL to support UDFs
(user-defined functions). See
http://dev.mysql.com/doc/refman/5.5/en/adding-functions.html. A UDF can
be regarded as an external stored function. Stored functions share
their namespace with UDFs. See
http://dev.mysql.com/doc/refman/5.5/en/function-resolution.html, for
the rules describing how the server interprets references to different
kinds of functions.

To invoke a stored procedure, use the CALL statement (see [HELP CALL]).
To invoke a stored function, refer to it in an expression. The function
returns a value during expression evaluation.

CREATE PROCEDURE and CREATE FUNCTION require the CREATE ROUTINE
privilege. They might also require the SUPER privilege, depending on
the DEFINER value, as described later in this section. If binary
logging is enabled, CREATE FUNCTION might require the SUPER privilege,
as described in
http://dev.mysql.com/doc/refman/5.5/en/stored-programs-logging.html.

By default, MySQL automatically grants the ALTER ROUTINE and EXECUTE
privileges to the routine creator. This behavior can be changed by
disabling the automatic_sp_privileges system variable. See
http://dev.mysql.com/doc/refman/5.5/en/stored-routines-privileges.html.

The DEFINER and SQL SECURITY clauses specify the security context to be
used when checking access privileges at routine execution time, as
described later in this section.

If the routine name is the same as the name of a built-in SQL function,
a syntax error occurs unless you use a space between the name and the
following parenthesis when defining the routine or invoking it later.
For this reason, avoid using the names of existing SQL functions for
your own stored routines.

The IGNORE_SPACE SQL mode applies to built-in functions, not to stored
routines. It is always permissible to have spaces after a stored
routine name, regardless of whether IGNORE_SPACE is enabled.

The parameter list enclosed within parentheses must always be present.
If there are no parameters, an empty parameter list of () should be
used. Parameter names are not case sensitive.

Each parameter is an IN parameter by default. To specify otherwise for
a parameter, use the keyword OUT or INOUT before the parameter name.

*Note*: Specifying a parameter as IN, OUT, or INOUT is valid only for a
PROCEDURE. For a FUNCTION, parameters are always regarded as IN
parameters.

An IN parameter passes a value into a procedure. The procedure might
modify the value, but the modification is not visible to the caller
when the procedure returns. An OUT parameter passes a value from the
procedure back to the caller. Its initial value is NULL within the
procedure, and its value is visible to the caller when the procedure
returns. An INOUT parameter is initialized by the caller, can be
modified by the procedure, and any change made by the procedure is
visible to the caller when the procedure returns.

For each OUT or INOUT parameter, pass a user-defined variable in the
CALL statement that invokes the procedure so that you can obtain its
value when the procedure returns. If you are calling the procedure from
within another stored procedure or function, you can also pass a
routine parameter or local routine variable as an IN or INOUT
parameter.

The following example shows a simple stored procedure that uses an OUT
parameter:

mysql&gt; delimiter //

mysql&gt; CREATE PROCEDURE simpleproc (OUT param1 INT)
    -&gt; BEGIN
    -&gt;   SELECT COUNT(*) INTO param1 FROM t;
    -&gt; END//
Query OK, 0 rows affected (0.00 sec)

mysql&gt; delimiter ;

mysql&gt; CALL simpleproc(@a);
Query OK, 0 rows affected (0.00 sec)

mysql&gt; SELECT @a;
+------+
| @a   |
+------+
| 3    |
+------+
1 row in set (0.00 sec)

The example uses the mysql client delimiter command to change the
statement delimiter from ; to // while the procedure is being defined.
This enables the ; delimiter used in the procedure body to be passed
through to the server rather than being interpreted by mysql itself.
See
http://dev.mysql.com/doc/refman/5.5/en/stored-programs-defining.html.

The RETURNS clause may be specified only for a FUNCTION, for which it
is mandatory. It indicates the return type of the function, and the
function body must contain a RETURN value statement. If the RETURN
statement returns a value of a different type, the value is coerced to
the proper type. For example, if a function specifies an ENUM or SET
value in the RETURNS clause, but the RETURN statement returns an
integer, the value returned from the function is the string for the
corresponding ENUM member of set of SET members.

The following example function takes a parameter, performs an operation
using an SQL function, and returns the result. In this case, it is
unnecessary to use delimiter because the function definition contains
no internal ; statement delimiters:

mysql&gt; CREATE FUNCTION hello (s CHAR(20))
mysql&gt; RETURNS CHAR(50) DETERMINISTIC
    -&gt; RETURN CONCAT('Hello, ',s,'!');
Query OK, 0 rows affected (0.00 sec)

mysql&gt; SELECT hello('world');
+----------------+
| hello('world') |
+----------------+
| Hello, world!  |
+----------------+
1 row in set (0.00 sec)

Parameter types and function return types can be declared to use any
valid data type, except that the COLLATE attribute cannot be used prior
to MySQL 5.5.3. As of 5.5.3, COLLATE can be used if preceded by the
CHARACTER SET attribute.

The routine_body consists of a valid SQL routine statement. This can be
a simple statement such as SELECT or INSERT, or a compound statement
written using BEGIN and END. Compound statements can contain
declarations, loops, and other control structure statements. The syntax
for these statements is described in
http://dev.mysql.com/doc/refman/5.5/en/sql-syntax-compound-statements.h
tml.

MySQL permits routines to contain DDL statements, such as CREATE and
DROP. MySQL also permits stored procedures (but not stored functions)
to contain SQL transaction statements such as COMMIT. Stored functions
may not contain statements that perform explicit or implicit commit or
rollback. Support for these statements is not required by the SQL
standard, which states that each DBMS vendor may decide whether to
permit them.

Statements that return a result set can be used within a stored
procedure but not within a stored function. This prohibition includes
SELECT statements that do not have an INTO var_list clause and other
statements such as SHOW, EXPLAIN, and CHECK TABLE. For statements that
can be determined at function definition time to return a result set, a
Not allowed to return a result set from a function error occurs
(ER_SP_NO_RETSET). For statements that can be determined only at
runtime to return a result set, a PROCEDURE %s can't return a result
set in the given context error occurs (ER_SP_BADSELECT).

USE statements within stored routines are not permitted. When a routine
is invoked, an implicit USE db_name is performed (and undone when the
routine terminates). The causes the routine to have the given default
database while it executes. References to objects in databases other
than the routine default database should be qualified with the
appropriate database name.

For additional information about statements that are not permitted in
stored routines, see
http://dev.mysql.com/doc/refman/5.5/en/stored-program-restrictions.html
.

For information about invoking stored procedures from within programs
written in a language that has a MySQL interface, see [HELP CALL].

MySQL stores the sql_mode system variable setting that is in effect at
the time a routine is created, and always executes the routine with
this setting in force, regardless of the server SQL mode in effect when
the routine is invoked.

The switch from the SQL mode of the invoker to that of the routine
occurs after evaluation of arguments and assignment of the resulting
values to routine parameters. If you define a routine in strict SQL
mode but invoke it in nonstrict mode, assignment of arguments to
routine parameters does not take place in strict mode. If you require
that expressions passed to a routine be assigned in strict SQL mode,
you should invoke the routine with strict mode in effect.

URL: http://dev.mysql.com/doc/refman/5.5/en/create-procedure.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-procedure.html</field>
	</row>
	<row>
		<field name="help_topic_id">305</field>
		<field name="name">VARBINARY</field>
		<field name="help_category_id">23</field>
		<field name="description">VARBINARY(M)

The VARBINARY type is similar to the VARCHAR type, but stores binary
byte strings rather than nonbinary character strings. M represents the
maximum column length in bytes.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">306</field>
		<field name="name">LOAD INDEX</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
LOAD INDEX INTO CACHE
  tbl_index_list [, tbl_index_list] ...

tbl_index_list:
  tbl_name
    [PARTITION (partition_list | ALL)]
    [[INDEX|KEY] (index_name[, index_name] ...)]
    [IGNORE LEAVES]

partition_list:
    partition_name[, partition_name][, ...]

The LOAD INDEX INTO CACHE statement preloads a table index into the key
cache to which it has been assigned by an explicit CACHE INDEX
statement, or into the default key cache otherwise.

LOAD INDEX INTO CACHE is used only for MyISAM tables. In MySQL 5.5, it
is also supported for partitioned MyISAM tables; in addition, indexes
on partitioned tables can be preloaded for one, several, or all
partitions.

The IGNORE LEAVES modifier causes only blocks for the nonleaf nodes of
the index to be preloaded.

IGNORE LEAVES is also supported for partitioned MyISAM tables.

URL: http://dev.mysql.com/doc/refman/5.5/en/load-index.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/load-index.html</field>
	</row>
	<row>
		<field name="help_topic_id">307</field>
		<field name="name">UNION</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
SELECT ...
UNION [ALL | DISTINCT] SELECT ...
[UNION [ALL | DISTINCT] SELECT ...]

UNION is used to combine the result from multiple SELECT statements
into a single result set.

The column names from the first SELECT statement are used as the column
names for the results returned. Selected columns listed in
corresponding positions of each SELECT statement should have the same
data type. (For example, the first column selected by the first
statement should have the same type as the first column selected by the
other statements.)

URL: http://dev.mysql.com/doc/refman/5.5/en/union.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/union.html</field>
	</row>
	<row>
		<field name="help_topic_id">308</field>
		<field name="name">TO_DAYS</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
TO_DAYS(date)

Given a date date, returns a day number (the number of days since year
0).

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT TO_DAYS(950501);
        -&gt; 728779
mysql&gt; SELECT TO_DAYS('2007-10-07');
        -&gt; 733321
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">309</field>
		<field name="name">NOT REGEXP</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
expr NOT REGEXP pat, expr NOT RLIKE pat

This is the same as NOT (expr REGEXP pat).

URL: http://dev.mysql.com/doc/refman/5.5/en/regexp.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/regexp.html</field>
	</row>
	<row>
		<field name="help_topic_id">310</field>
		<field name="name">SHOW INDEX</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW {INDEX | INDEXES | KEYS}
    {FROM | IN} tbl_name
    [{FROM | IN} db_name]
    [WHERE expr]

SHOW INDEX returns table index information. The format resembles that
of the SQLStatistics call in ODBC. This statement requires some
privilege for any column in the table.
You can use db_name.tbl_name as an alternative to the tbl_name FROM
db_name syntax. These two statements are equivalent:

SHOW INDEX FROM mytable FROM mydb;
SHOW INDEX FROM mydb.mytable;

The WHERE clause can be given to select rows using more general
conditions, as discussed in
http://dev.mysql.com/doc/refman/5.5/en/extended-show.html.

You can also list a table's indexes with the mysqlshow -k db_name
tbl_name command.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-index.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-index.html</field>
	</row>
	<row>
		<field name="help_topic_id">311</field>
		<field name="name">SHOW CREATE DATABASE</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW CREATE {DATABASE | SCHEMA} [IF NOT EXISTS] db_name

Shows the CREATE DATABASE statement that creates the given database. If
the SHOW statement includes an IF NOT EXISTS clause, the output too
includes such a clause. SHOW CREATE SCHEMA is a synonym for SHOW CREATE
DATABASE.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-create-database.html

</field>
		<field name="example">mysql&gt; SHOW CREATE DATABASE test\G
*************************** 1. row ***************************
       Database: test
Create Database: CREATE DATABASE `test`
                 /*!40100 DEFAULT CHARACTER SET latin1 */

mysql&gt; SHOW CREATE SCHEMA test\G
*************************** 1. row ***************************
       Database: test
Create Database: CREATE DATABASE `test`
                 /*!40100 DEFAULT CHARACTER SET latin1 */
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-create-database.html</field>
	</row>
	<row>
		<field name="help_topic_id">312</field>
		<field name="name">LEAVE</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
LEAVE label

This statement is used to exit the flow control construct that has the
given label. If the label is for the outermost stored program block,
LEAVE exits the program.

LEAVE can be used within BEGIN ... END or loop constructs (LOOP,
REPEAT, WHILE).

URL: http://dev.mysql.com/doc/refman/5.5/en/leave.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/leave.html</field>
	</row>
	<row>
		<field name="help_topic_id">313</field>
		<field name="name">NOT IN</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
expr NOT IN (value,...)

This is the same as NOT (expr IN (value,...)).

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">314</field>
		<field name="name">!</field>
		<field name="help_category_id">15</field>
		<field name="description">Syntax:
NOT, !

Logical NOT. Evaluates to 1 if the operand is 0, to 0 if the operand is
nonzero, and NOT NULL returns NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/logical-operators.html

</field>
		<field name="example">mysql&gt; SELECT NOT 10;
        -&gt; 0
mysql&gt; SELECT NOT 0;
        -&gt; 1
mysql&gt; SELECT NOT NULL;
        -&gt; NULL
mysql&gt; SELECT ! (1+1);
        -&gt; 0
mysql&gt; SELECT ! 1+1;
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/logical-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">315</field>
		<field name="name">DECLARE HANDLER</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
DECLARE handler_action HANDLER
    FOR condition_value [, condition_value] ...
    statement

handler_action:
    CONTINUE
  | EXIT
  | UNDO

condition_value:
    mysql_error_code
  | SQLSTATE [VALUE] sqlstate_value
  | condition_name
  | SQLWARNING
  | NOT FOUND
  | SQLEXCEPTION

The DECLARE ... HANDLER statement specifies a handler that deals with
one or more conditions. If one of these conditions occurs, the
specified statement executes. statement can be a simple statement such
as SET var_name = value, or a compound statement written using BEGIN
and END (see [HELP BEGIN END]).

Handler declarations must appear after variable or condition
declarations.

The handler_action value indicates what action the handler takes after
execution of the handler statement:

o CONTINUE: Execution of the current program continues.

o EXIT: Execution terminates for the BEGIN ... END compound statement
  in which the handler is declared. This is true even if the condition
  occurs in an inner block.

o UNDO: Not supported.

The condition_value for DECLARE ... HANDLER indicates the specific
condition or class of conditions that activates the handler:

o A MySQL error code (a number) or an SQLSTATE value (a 5-character
  string literal). You should not use MySQL error code 0 or SQLSTATE
  values that begin with '00', because those indicate success rather
  than an error condition. For a list of MySQL error codes and SQLSTATE
  values, see
  http://dev.mysql.com/doc/refman/5.5/en/error-messages-server.html.

o A condition name previously specified with DECLARE ... CONDITION. A
  condition name can be associated with a MySQL error code or SQLSTATE
  value. See [HELP DECLARE CONDITION].

o SQLWARNING is shorthand for the class of SQLSTATE values that begin
  with '01'.

o NOT FOUND is shorthand for the class of SQLSTATE values that begin
  with '02'. This is relevant within the context of cursors and is used
  to control what happens when a cursor reaches the end of a data set.
  If no more rows are available, a No Data condition occurs with
  SQLSTATE value '02000'. To detect this condition, you can set up a
  handler for it (or for a NOT FOUND condition). For an example, see
  http://dev.mysql.com/doc/refman/5.5/en/cursors.html. This condition
  also occurs for SELECT ... INTO var_list statements that retrieve no
  rows.

o SQLEXCEPTION is shorthand for the class of SQLSTATE values that do
  not begin with '00', '01', or '02'.

If a condition occurs for which no handler has been declared, the
action taken depends on the condition class:

o For SQLEXCEPTION conditions, the stored program terminates at the
  statement that raised the condition, as if there were an EXIT
  handler. If the program was called by another stored program, the
  calling program handles the condition using the handler selection
  rules applied to its own handlers.

o For SQLWARNING conditions, the program continues executing, as if
  there were a CONTINUE handler.

o For NOT FOUND conditions, if the condition was raised normally, the
  action is CONTINUE. If it was raised by SIGNAL or RESIGNAL, the
  action is EXIT.

URL: http://dev.mysql.com/doc/refman/5.5/en/declare-handler.html

</field>
		<field name="example">mysql&gt; CREATE TABLE test.t (s1 INT, PRIMARY KEY (s1));
Query OK, 0 rows affected (0.00 sec)

mysql&gt; delimiter //

mysql&gt; CREATE PROCEDURE handlerdemo ()
    -&gt; BEGIN
    -&gt;   DECLARE CONTINUE HANDLER FOR SQLSTATE '23000' SET @x2 = 1;
    -&gt;   SET @x = 1;
    -&gt;   INSERT INTO test.t VALUES (1);
    -&gt;   SET @x = 2;
    -&gt;   INSERT INTO test.t VALUES (1);
    -&gt;   SET @x = 3;
    -&gt; END;
    -&gt; //
Query OK, 0 rows affected (0.00 sec)

mysql&gt; CALL handlerdemo()//
Query OK, 0 rows affected (0.00 sec)

mysql&gt; SELECT @x//
    +------+
    | @x   |
    +------+
    | 3    |
    +------+
    1 row in set (0.00 sec)
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/declare-handler.html</field>
	</row>
	<row>
		<field name="help_topic_id">316</field>
		<field name="name">DOUBLE</field>
		<field name="help_category_id">23</field>
		<field name="description">DOUBLE[(M,D)] [UNSIGNED] [ZEROFILL]

A normal-size (double-precision) floating-point number. Permissible
values are -1.7976931348623157E+308 to -2.2250738585072014E-308, 0, and
2.2250738585072014E-308 to 1.7976931348623157E+308. These are the
theoretical limits, based on the IEEE standard. The actual range might
be slightly smaller depending on your hardware or operating system.

M is the total number of digits and D is the number of digits following
the decimal point. If M and D are omitted, values are stored to the
limits permitted by the hardware. A double-precision floating-point
number is accurate to approximately 15 decimal places.

UNSIGNED, if specified, disallows negative values.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">317</field>
		<field name="name">TIME</field>
		<field name="help_category_id">23</field>
		<field name="description">TIME

A time. The range is '-838:59:59' to '838:59:59'. MySQL displays TIME
values in 'HH:MM:SS' format, but permits assignment of values to TIME
columns using either strings or numbers.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">318</field>
		<field name="name">&amp;&amp;</field>
		<field name="help_category_id">15</field>
		<field name="description">Syntax:
AND, &amp;&amp;

Logical AND. Evaluates to 1 if all operands are nonzero and not NULL,
to 0 if one or more operands are 0, otherwise NULL is returned.

URL: http://dev.mysql.com/doc/refman/5.5/en/logical-operators.html

</field>
		<field name="example">mysql&gt; SELECT 1 &amp;&amp; 1;
        -&gt; 1
mysql&gt; SELECT 1 &amp;&amp; 0;
        -&gt; 0
mysql&gt; SELECT 1 &amp;&amp; NULL;
        -&gt; NULL
mysql&gt; SELECT 0 &amp;&amp; NULL;
        -&gt; 0
mysql&gt; SELECT NULL &amp;&amp; 0;
        -&gt; 0
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/logical-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">319</field>
		<field name="name">X</field>
		<field name="help_category_id">11</field>
		<field name="description">X(p)

Returns the X-coordinate value for the Point object p as a
double-precision number.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SELECT X(POINT(56.7, 53.34));
+-----------------------+
| X(POINT(56.7, 53.34)) |
+-----------------------+
|                  56.7 |
+-----------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">320</field>
		<field name="name">SYSTEM_USER</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
SYSTEM_USER()

SYSTEM_USER() is a synonym for USER().

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">321</field>
		<field name="name">FOUND_ROWS</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
FOUND_ROWS()

A SELECT statement may include a LIMIT clause to restrict the number of
rows the server returns to the client. In some cases, it is desirable
to know how many rows the statement would have returned without the
LIMIT, but without running the statement again. To obtain this row
count, include a SQL_CALC_FOUND_ROWS option in the SELECT statement,
and then invoke FOUND_ROWS() afterward:

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example">mysql&gt; SELECT SQL_CALC_FOUND_ROWS * FROM tbl_name
    -&gt; WHERE id &gt; 100 LIMIT 10;
mysql&gt; SELECT FOUND_ROWS();
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">322</field>
		<field name="name">CROSSES</field>
		<field name="help_category_id">31</field>
		<field name="description">Crosses(g1,g2)

Returns 1 if g1 spatially crosses g2. Returns NULL if g1 is a Polygon
or a MultiPolygon, or if g2 is a Point or a MultiPoint. Otherwise,
returns 0.

The term spatially crosses denotes a spatial relation between two given
geometries that has the following properties:

o The two geometries intersect

o Their intersection results in a geometry that has a dimension that is
  one less than the maximum dimension of the two given geometries

o Their intersection is not equal to either of the two given geometries

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">323</field>
		<field name="name">TRUNCATE TABLE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
TRUNCATE [TABLE] tbl_name

TRUNCATE TABLE empties a table completely. It requires the DROP
privilege.

Logically, TRUNCATE TABLE is similar to a DELETE statement that deletes
all rows, or a sequence of DROP TABLE and CREATE TABLE statements. To
achieve high performance, it bypasses the DML method of deleting data.
Thus, it cannot be rolled back, it does not cause ON DELETE triggers to
fire, and it cannot be performed for InnoDB tables with parent-child
foreign key relationships.

Although TRUNCATE TABLE is similar to DELETE, it is classified as a DDL
statement rather than a DML statement. It differs from DELETE in the
following ways in MySQL 5.5:

o Truncate operations drop and re-create the table, which is much
  faster than deleting rows one by one, particularly for large tables.

o Truncate operations cause an implicit commit, and so cannot be rolled
  back.

o Truncation operations cannot be performed if the session holds an
  active table lock.

o TRUNCATE TABLE fails for an InnoDB table if there are any FOREIGN KEY
  constraints from other tables that reference the table. Foreign key
  constraints between columns of the same table are permitted.

o Truncation operations do not return a meaningful value for the number
  of deleted rows. The usual result is &quot;0 rows affected,&quot; which should
  be interpreted as &quot;no information.&quot;

o As long as the table format file tbl_name.frm is valid, the table can
  be re-created as an empty table with TRUNCATE TABLE, even if the data
  or index files have become corrupted.

o Any AUTO_INCREMENT value is reset to its start value. This is true
  even for MyISAM and InnoDB, which normally do not reuse sequence
  values.

o When used with partitioned tables, TRUNCATE TABLE preserves the
  partitioning; that is, the data and index files are dropped and
  re-created, while the partition definitions (.par) file is
  unaffected.

o The TRUNCATE TABLE statement does not invoke ON DELETE triggers.

URL: http://dev.mysql.com/doc/refman/5.5/en/truncate-table.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/truncate-table.html</field>
	</row>
	<row>
		<field name="help_topic_id">324</field>
		<field name="name">BIT_XOR</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
BIT_XOR(expr)

Returns the bitwise XOR of all bits in expr. The calculation is
performed with 64-bit (BIGINT) precision.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">325</field>
		<field name="name">CURRENT_DATE</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
CURRENT_DATE, CURRENT_DATE()

CURRENT_DATE and CURRENT_DATE() are synonyms for CURDATE().

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">326</field>
		<field name="name">START SLAVE</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
START SLAVE [thread_types]

START SLAVE [SQL_THREAD] UNTIL
    MASTER_LOG_FILE = 'log_name', MASTER_LOG_POS = log_pos

START SLAVE [SQL_THREAD] UNTIL
    RELAY_LOG_FILE = 'log_name', RELAY_LOG_POS = log_pos

thread_types:
    [thread_type [, thread_type] ... ]

thread_type: IO_THREAD | SQL_THREAD

START SLAVE with no thread_type options starts both of the slave
threads. The I/O thread reads events from the master server and stores
them in the relay log. The SQL thread reads events from the relay log
and executes them. START SLAVE requires the SUPER privilege.

If START SLAVE succeeds in starting the slave threads, it returns
without any error. However, even in that case, it might be that the
slave threads start and then later stop (for example, because they do
not manage to connect to the master or read its binary log, or some
other problem). START SLAVE does not warn you about this. You must
check the slave's error log for error messages generated by the slave
threads, or check that they are running satisfactorily with SHOW SLAVE
STATUS.

URL: http://dev.mysql.com/doc/refman/5.5/en/start-slave.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/start-slave.html</field>
	</row>
	<row>
		<field name="help_topic_id">327</field>
		<field name="name">AREA</field>
		<field name="help_category_id">2</field>
		<field name="description">Area(poly)

Returns as a double-precision number the area of the Polygon value
poly, as measured in its spatial reference system.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SET @poly = 'Polygon((0 0,0 3,3 0,0 0),(1 1,1 2,2 1,1 1))';
mysql&gt; SELECT Area(GeomFromText(@poly));
+---------------------------+
| Area(GeomFromText(@poly)) |
+---------------------------+
|                         4 |
+---------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">328</field>
		<field name="name">FLUSH</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
FLUSH [NO_WRITE_TO_BINLOG | LOCAL]
    flush_option [, flush_option] ...

The FLUSH statement has several variant forms that clear or reload
various internal caches, flush tables, or acquire locks. To execute
FLUSH, you must have the RELOAD privilege. Specific flush options might
require additional privileges, as described later.

By default, the server writes FLUSH statements to the binary log so
that they replicate to replication slaves. To suppress logging, use the
optional NO_WRITE_TO_BINLOG keyword or its alias LOCAL.

*Note*: FLUSH LOGS, FLUSH MASTER, FLUSH SLAVE, and FLUSH TABLES WITH
READ LOCK (with or without a table list) are not written to the binary
log in any case because they would cause problems if replicated to a
slave.

The FLUSH statement causes an implicit commit. See
http://dev.mysql.com/doc/refman/5.5/en/implicit-commit.html.

The RESET statement is similar to FLUSH. See [HELP RESET], for
information about using the RESET statement with replication.

URL: http://dev.mysql.com/doc/refman/5.5/en/flush.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/flush.html</field>
	</row>
	<row>
		<field name="help_topic_id">329</field>
		<field name="name">BEGIN END</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
[begin_label:] BEGIN
    [statement_list]
END [end_label]

BEGIN ... END syntax is used for writing compound statements, which can
appear within stored programs (stored procedures and functions,
triggers, and events). A compound statement can contain multiple
statements, enclosed by the BEGIN and END keywords. statement_list
represents a list of one or more statements, each terminated by a
semicolon (;) statement delimiter. The statement_list itself is
optional, so the empty compound statement (BEGIN END) is legal.

BEGIN ... END blocks can be nested.

Use of multiple statements requires that a client is able to send
statement strings containing the ; statement delimiter. In the mysql
command-line client, this is handled with the delimiter command.
Changing the ; end-of-statement delimiter (for example, to //) permit ;
to be used in a program body. For an example, see
http://dev.mysql.com/doc/refman/5.5/en/stored-programs-defining.html.

A BEGIN ... END block can be labeled. See [HELP labels].

URL: http://dev.mysql.com/doc/refman/5.5/en/begin-end.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/begin-end.html</field>
	</row>
	<row>
		<field name="help_topic_id">330</field>
		<field name="name">SHOW PROCEDURE STATUS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW PROCEDURE STATUS
    [LIKE 'pattern' | WHERE expr]

This statement is a MySQL extension. It returns characteristics of a
stored procedure, such as the database, name, type, creator, creation
and modification dates, and character set information. A similar
statement, SHOW FUNCTION STATUS, displays information about stored
functions (see [HELP SHOW FUNCTION STATUS]).

The LIKE clause, if present, indicates which procedure or function
names to match. The WHERE clause can be given to select rows using more
general conditions, as discussed in
http://dev.mysql.com/doc/refman/5.5/en/extended-show.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-procedure-status.html

</field>
		<field name="example">mysql&gt; SHOW PROCEDURE STATUS LIKE 'sp1'\G
*************************** 1. row ***************************
                  Db: test
                Name: sp1
                Type: PROCEDURE
             Definer: testuser@localhost
            Modified: 2004-08-03 15:29:37
             Created: 2004-08-03 15:29:37
       Security_type: DEFINER
             Comment:
character_set_client: latin1
collation_connection: latin1_swedish_ci
  Database Collation: latin1_swedish_ci
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-procedure-status.html</field>
	</row>
	<row>
		<field name="help_topic_id">331</field>
		<field name="name">SHOW WARNINGS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW WARNINGS [LIMIT [offset,] row_count]
SHOW COUNT(*) WARNINGS

SHOW WARNINGS shows information about the conditions (errors, warnings,
and notes) that resulted from the last statement in the current session
that generated messages. It shows nothing if the last statement used a
table and generated no messages. (That is, a statement that uses a
table but generates no messages clears the message list.) Statements
that do not use tables and do not generate messages have no effect on
the message list.

Warnings are generated for DML statements such as INSERT, UPDATE, and
LOAD DATA INFILE as well as DDL statements such as CREATE TABLE and
ALTER TABLE.

SHOW WARNINGS is also used following EXPLAIN EXTENDED, to display the
extra information generated by EXPLAIN when the EXTENDED keyword is
used. See http://dev.mysql.com/doc/refman/5.5/en/explain-extended.html.

The LIMIT clause has the same syntax as for the SELECT statement. See
http://dev.mysql.com/doc/refman/5.5/en/select.html.

A related statement, SHOW ERRORS, shows only the error conditions (it
excludes warnings and notes). See [HELP SHOW ERRORS].

The SHOW COUNT(*) WARNINGS statement displays the total number of
errors, warnings, and notes. You can also retrieve this number from the
warning_count system variable:

SHOW COUNT(*) WARNINGS;
SELECT @@warning_count;

URL: http://dev.mysql.com/doc/refman/5.5/en/show-warnings.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-warnings.html</field>
	</row>
	<row>
		<field name="help_topic_id">332</field>
		<field name="name">DROP USER</field>
		<field name="help_category_id">10</field>
		<field name="description">Syntax:
DROP USER user [, user] ...

The DROP USER statement removes one or more MySQL accounts and their
privileges. It removes privilege rows for the account from all grant
tables. To use this statement, you must have the global CREATE USER
privilege or the DELETE privilege for the mysql database. Each account
name uses the format described in
http://dev.mysql.com/doc/refman/5.5/en/account-names.html. For example:

DROP USER 'jeffrey'@'localhost';

If you specify only the user name part of the account name, a host name
part of '%' is used.

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-user.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-user.html</field>
	</row>
	<row>
		<field name="help_topic_id">333</field>
		<field name="name">STDDEV_POP</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
STDDEV_POP(expr)

Returns the population standard deviation of expr (the square root of
VAR_POP()). You can also use STD() or STDDEV(), which are equivalent
but not standard SQL.

STDDEV_POP() returns NULL if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">334</field>
		<field name="name">SHOW CHARACTER SET</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW CHARACTER SET
    [LIKE 'pattern' | WHERE expr]

The SHOW CHARACTER SET statement shows all available character sets.
The LIKE clause, if present, indicates which character set names to
match. The WHERE clause can be given to select rows using more general
conditions, as discussed in
http://dev.mysql.com/doc/refman/5.5/en/extended-show.html. For example:

mysql&gt; SHOW CHARACTER SET LIKE 'latin%';
+---------+-----------------------------+-------------------+--------+
| Charset | Description                 | Default collation | Maxlen |
+---------+-----------------------------+-------------------+--------+
| latin1  | cp1252 West European        | latin1_swedish_ci |      1 |
| latin2  | ISO 8859-2 Central European | latin2_general_ci |      1 |
| latin5  | ISO 8859-9 Turkish          | latin5_turkish_ci |      1 |
| latin7  | ISO 8859-13 Baltic          | latin7_general_ci |      1 |
+---------+-----------------------------+-------------------+--------+

URL: http://dev.mysql.com/doc/refman/5.5/en/show-character-set.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-character-set.html</field>
	</row>
	<row>
		<field name="help_topic_id">335</field>
		<field name="name">SUBSTRING</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
SUBSTRING(str,pos), SUBSTRING(str FROM pos), SUBSTRING(str,pos,len),
SUBSTRING(str FROM pos FOR len)

The forms without a len argument return a substring from string str
starting at position pos. The forms with a len argument return a
substring len characters long from string str, starting at position
pos. The forms that use FROM are standard SQL syntax. It is also
possible to use a negative value for pos. In this case, the beginning
of the substring is pos characters from the end of the string, rather
than the beginning. A negative value may be used for pos in any of the
forms of this function.

For all forms of SUBSTRING(), the position of the first character in
the string from which the substring is to be extracted is reckoned as
1.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT SUBSTRING('Quadratically',5);
        -&gt; 'ratically'
mysql&gt; SELECT SUBSTRING('foobarbar' FROM 4);
        -&gt; 'barbar'
mysql&gt; SELECT SUBSTRING('Quadratically',5,6);
        -&gt; 'ratica'
mysql&gt; SELECT SUBSTRING('Sakila', -3);
        -&gt; 'ila'
mysql&gt; SELECT SUBSTRING('Sakila', -5, 3);
        -&gt; 'aki'
mysql&gt; SELECT SUBSTRING('Sakila' FROM -4 FOR 2);
        -&gt; 'ki'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">336</field>
		<field name="name">ISEMPTY</field>
		<field name="help_category_id">37</field>
		<field name="description">IsEmpty(g)

This function is a placeholder that returns 0 for any valid geometry
value, 1 for any invalid geometry value or NULL.

MySQL does not support GIS EMPTY values such as POINT EMPTY.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">337</field>
		<field name="name">SHOW FUNCTION STATUS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW FUNCTION STATUS
    [LIKE 'pattern' | WHERE expr]

This statement is similar to SHOW PROCEDURE STATUS but for stored
functions. See [HELP SHOW PROCEDURE STATUS].

URL: http://dev.mysql.com/doc/refman/5.5/en/show-function-status.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-function-status.html</field>
	</row>
	<row>
		<field name="help_topic_id">338</field>
		<field name="name">LTRIM</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
LTRIM(str)

Returns the string str with leading space characters removed.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT LTRIM('  barbar');
        -&gt; 'barbar'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">339</field>
		<field name="name">INTERSECTS</field>
		<field name="help_category_id">31</field>
		<field name="description">Intersects(g1,g2)

Returns 1 or 0 to indicate whether g1 spatially intersects g2.

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">340</field>
		<field name="name">CALL</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
CALL sp_name([parameter[,...]])
CALL sp_name[()]

The CALL statement invokes a stored procedure that was defined
previously with CREATE PROCEDURE.

Stored procedures that take no arguments can be invoked without
parentheses. That is, CALL p() and CALL p are equivalent.

CALL can pass back values to its caller using parameters that are
declared as OUT or INOUT parameters. When the procedure returns, a
client program can also obtain the number of rows affected for the
final statement executed within the routine: At the SQL level, call the
ROW_COUNT() function; from the C API, call the mysql_affected_rows()
function.

URL: http://dev.mysql.com/doc/refman/5.5/en/call.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/call.html</field>
	</row>
	<row>
		<field name="help_topic_id">341</field>
		<field name="name">MBRDISJOINT</field>
		<field name="help_category_id">6</field>
		<field name="description">MBRDisjoint(g1,g2)

Returns 1 or 0 to indicate whether the Minimum Bounding Rectangles of
the two geometries g1 and g2 are disjoint (do not intersect).

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">342</field>
		<field name="name">VALUES</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
VALUES(col_name)

In an INSERT ... ON DUPLICATE KEY UPDATE statement, you can use the
VALUES(col_name) function in the UPDATE clause to refer to column
values from the INSERT portion of the statement. In other words,
VALUES(col_name) in the UPDATE clause refers to the value of col_name
that would be inserted, had no duplicate-key conflict occurred. This
function is especially useful in multiple-row inserts. The VALUES()
function is meaningful only in the ON DUPLICATE KEY UPDATE clause of
INSERT statements and returns NULL otherwise. See
http://dev.mysql.com/doc/refman/5.5/en/insert-on-duplicate.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example">mysql&gt; INSERT INTO table (a,b,c) VALUES (1,2,3),(4,5,6)
    -&gt; ON DUPLICATE KEY UPDATE c=VALUES(a)+VALUES(b);
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">343</field>
		<field name="name">SUBSTRING_INDEX</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
SUBSTRING_INDEX(str,delim,count)

Returns the substring from string str before count occurrences of the
delimiter delim. If count is positive, everything to the left of the
final delimiter (counting from the left) is returned. If count is
negative, everything to the right of the final delimiter (counting from
the right) is returned. SUBSTRING_INDEX() performs a case-sensitive
match when searching for delim.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT SUBSTRING_INDEX('www.mysql.com', '.', 2);
        -&gt; 'www.mysql'
mysql&gt; SELECT SUBSTRING_INDEX('www.mysql.com', '.', -2);
        -&gt; 'mysql.com'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">344</field>
		<field name="name">ENCODE</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
ENCODE(str,pass_str)

Encrypt str using pass_str as the password. To decrypt the result, use
DECODE().

The result is a binary string of the same length as str.

The strength of the encryption is based on how good the random
generator is. It should suffice for short strings.

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">345</field>
		<field name="name">LOOP</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
[begin_label:] LOOP
    statement_list
END LOOP [end_label]

LOOP implements a simple loop construct, enabling repeated execution of
the statement list, which consists of one or more statements, each
terminated by a semicolon (;) statement delimiter. The statements
within the loop are repeated until the loop is terminated. Usually,
this is accomplished with a LEAVE statement. Within a stored function,
RETURN can also be used, which exits the function entirely.

Neglecting to include a loop-termination statement results in an
infinite loop.

A LOOP statement can be labeled. For the rules regarding label use, see
[HELP labels].

URL: http://dev.mysql.com/doc/refman/5.5/en/loop.html

</field>
		<field name="example">CREATE PROCEDURE doiterate(p1 INT)
BEGIN
  label1: LOOP
    SET p1 = p1 + 1;
    IF p1 &lt; 10 THEN
      ITERATE label1;
    END IF;
    LEAVE label1;
  END LOOP label1;
  SET @x = p1;
END;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/loop.html</field>
	</row>
	<row>
		<field name="help_topic_id">346</field>
		<field name="name">TRUNCATE</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
TRUNCATE(X,D)

Returns the number X, truncated to D decimal places. If D is 0, the
result has no decimal point or fractional part. D can be negative to
cause D digits left of the decimal point of the value X to become zero.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT TRUNCATE(1.223,1);
        -&gt; 1.2
mysql&gt; SELECT TRUNCATE(1.999,1);
        -&gt; 1.9
mysql&gt; SELECT TRUNCATE(1.999,0);
        -&gt; 1
mysql&gt; SELECT TRUNCATE(-1.999,1);
        -&gt; -1.9
mysql&gt; SELECT TRUNCATE(122,-2);
       -&gt; 100
mysql&gt; SELECT TRUNCATE(10.28*100,0);
       -&gt; 1028
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">347</field>
		<field name="name">TIMESTAMPADD</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
TIMESTAMPADD(unit,interval,datetime_expr)

Adds the integer expression interval to the date or datetime expression
datetime_expr. The unit for interval is given by the unit argument,
which should be one of the following values: MICROSECOND
(microseconds), SECOND, MINUTE, HOUR, DAY, WEEK, MONTH, QUARTER, or
YEAR.

It is possible to use FRAC_SECOND in place of MICROSECOND, but
FRAC_SECOND is deprecated. FRAC_SECOND was removed in MySQL 5.5.3.

The unit value may be specified using one of keywords as shown, or with
a prefix of SQL_TSI_. For example, DAY and SQL_TSI_DAY both are legal.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT TIMESTAMPADD(MINUTE,1,'2003-01-02');
        -&gt; '2003-01-02 00:01:00'
mysql&gt; SELECT TIMESTAMPADD(WEEK,1,'2003-01-02');
        -&gt; '2003-01-09'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">348</field>
		<field name="name">SHOW</field>
		<field name="help_category_id">27</field>
		<field name="description">SHOW has many forms that provide information about databases, tables,
columns, or status information about the server. This section describes
those following:

SHOW AUTHORS
SHOW {BINARY | MASTER} LOGS
SHOW BINLOG EVENTS [IN 'log_name'] [FROM pos] [LIMIT [offset,] row_count]
SHOW CHARACTER SET [like_or_where]
SHOW COLLATION [like_or_where]
SHOW [FULL] COLUMNS FROM tbl_name [FROM db_name] [like_or_where]
SHOW CONTRIBUTORS
SHOW CREATE DATABASE db_name
SHOW CREATE EVENT event_name
SHOW CREATE FUNCTION func_name
SHOW CREATE PROCEDURE proc_name
SHOW CREATE TABLE tbl_name
SHOW CREATE TRIGGER trigger_name
SHOW CREATE VIEW view_name
SHOW DATABASES [like_or_where]
SHOW ENGINE engine_name {STATUS | MUTEX}
SHOW [STORAGE] ENGINES
SHOW ERRORS [LIMIT [offset,] row_count]
SHOW EVENTS
SHOW FUNCTION CODE func_name
SHOW FUNCTION STATUS [like_or_where]
SHOW GRANTS FOR user
SHOW INDEX FROM tbl_name [FROM db_name]
SHOW MASTER STATUS
SHOW OPEN TABLES [FROM db_name] [like_or_where]
SHOW PLUGINS
SHOW PROCEDURE CODE proc_name
SHOW PROCEDURE STATUS [like_or_where]
SHOW PRIVILEGES
SHOW [FULL] PROCESSLIST
SHOW PROFILE [types] [FOR QUERY n] [OFFSET n] [LIMIT n]
SHOW PROFILES
SHOW SLAVE HOSTS
SHOW SLAVE STATUS
SHOW [GLOBAL | SESSION] STATUS [like_or_where]
SHOW TABLE STATUS [FROM db_name] [like_or_where]
SHOW [FULL] TABLES [FROM db_name] [like_or_where]
SHOW TRIGGERS [FROM db_name] [like_or_where]
SHOW [GLOBAL | SESSION] VARIABLES [like_or_where]
SHOW WARNINGS [LIMIT [offset,] row_count]

like_or_where:
    LIKE 'pattern'
  | WHERE expr

If the syntax for a given SHOW statement includes a LIKE 'pattern'
part, 'pattern' is a string that can contain the SQL &quot;%&quot; and &quot;_&quot;
wildcard characters. The pattern is useful for restricting statement
output to matching values.

Several SHOW statements also accept a WHERE clause that provides more
flexibility in specifying which rows to display. See
http://dev.mysql.com/doc/refman/5.5/en/extended-show.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/show.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show.html</field>
	</row>
	<row>
		<field name="help_topic_id">349</field>
		<field name="name">GREATEST</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
GREATEST(value1,value2,...)

With two or more arguments, returns the largest (maximum-valued)
argument. The arguments are compared using the same rules as for
LEAST().

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT GREATEST(2,0);
        -&gt; 2
mysql&gt; SELECT GREATEST(34.0,3.0,5.0,767.0);
        -&gt; 767.0
mysql&gt; SELECT GREATEST('B','A','C');
        -&gt; 'C'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">350</field>
		<field name="name">SHOW VARIABLES</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW [GLOBAL | SESSION] VARIABLES
    [LIKE 'pattern' | WHERE expr]

SHOW VARIABLES shows the values of MySQL system variables. This
information also can be obtained using the mysqladmin variables
command. The LIKE clause, if present, indicates which variable names to
match. The WHERE clause can be given to select rows using more general
conditions, as discussed in
http://dev.mysql.com/doc/refman/5.5/en/extended-show.html. This
statement does not require any privilege. It requires only the ability
to connect to the server.

With the GLOBAL modifier, SHOW VARIABLES displays the values that are
used for new connections to MySQL. As of MySQL 5.5.3, if a variable has
no global value, no value is displayed. Before 5.5.3, the session value
is displayed. With SESSION, SHOW VARIABLES displays the values that are
in effect for the current connection. If no modifier is present, the
default is SESSION. LOCAL is a synonym for SESSION.
With a LIKE clause, the statement displays only rows for those
variables with names that match the pattern. To obtain the row for a
specific variable, use a LIKE clause as shown:

SHOW VARIABLES LIKE 'max_join_size';
SHOW SESSION VARIABLES LIKE 'max_join_size';

To get a list of variables whose name match a pattern, use the &quot;%&quot;
wildcard character in a LIKE clause:

SHOW VARIABLES LIKE '%size%';
SHOW GLOBAL VARIABLES LIKE '%size%';

Wildcard characters can be used in any position within the pattern to
be matched. Strictly speaking, because &quot;_&quot; is a wildcard that matches
any single character, you should escape it as &quot;\_&quot; to match it
literally. In practice, this is rarely necessary.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-variables.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-variables.html</field>
	</row>
	<row>
		<field name="help_topic_id">351</field>
		<field name="name">BINLOG</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
BINLOG 'str'

BINLOG is an internal-use statement. It is generated by the mysqlbinlog
program as the printable representation of certain events in binary log
files. (See http://dev.mysql.com/doc/refman/5.5/en/mysqlbinlog.html.)
The 'str' value is a base 64-encoded string the that server decodes to
determine the data change indicated by the corresponding event. This
statement requires the SUPER privilege.

URL: http://dev.mysql.com/doc/refman/5.5/en/binlog.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/binlog.html</field>
	</row>
	<row>
		<field name="help_topic_id">352</field>
		<field name="name">BIT_AND</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
BIT_AND(expr)

Returns the bitwise AND of all bits in expr. The calculation is
performed with 64-bit (BIGINT) precision.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">353</field>
		<field name="name">SECOND</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
SECOND(time)

Returns the second for time, in the range 0 to 59.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT SECOND('10:05:03');
        -&gt; 3
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">354</field>
		<field name="name">ATAN2</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
ATAN(Y,X), ATAN2(Y,X)

Returns the arc tangent of the two variables X and Y. It is similar to
calculating the arc tangent of Y / X, except that the signs of both
arguments are used to determine the quadrant of the result.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT ATAN(-2,2);
        -&gt; -0.78539816339745
mysql&gt; SELECT ATAN2(PI(),0);
        -&gt; 1.5707963267949
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">355</field>
		<field name="name">MBRCONTAINS</field>
		<field name="help_category_id">6</field>
		<field name="description">MBRContains(g1,g2)

Returns 1 or 0 to indicate whether the Minimum Bounding Rectangle of g1
contains the Minimum Bounding Rectangle of g2. This tests the opposite
relationship as MBRWithin().

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example">mysql&gt; SET @g1 = GeomFromText('Polygon((0 0,0 3,3 3,3 0,0 0))');
mysql&gt; SET @g2 = GeomFromText('Point(1 1)');
mysql&gt; SELECT MBRContains(@g1,@g2), MBRContains(@g2,@g1);
----------------------+----------------------+
| MBRContains(@g1,@g2) | MBRContains(@g2,@g1) |
+----------------------+----------------------+
|                    1 |                    0 |
+----------------------+----------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">356</field>
		<field name="name">HOUR</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
HOUR(time)

Returns the hour for time. The range of the return value is 0 to 23 for
time-of-day values. However, the range of TIME values actually is much
larger, so HOUR can return values greater than 23.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT HOUR('10:05:03');
        -&gt; 10
mysql&gt; SELECT HOUR('272:59:59');
        -&gt; 272
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">357</field>
		<field name="name">SELECT</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
SELECT
    [ALL | DISTINCT | DISTINCTROW ]
      [HIGH_PRIORITY]
      [STRAIGHT_JOIN]
      [SQL_SMALL_RESULT] [SQL_BIG_RESULT] [SQL_BUFFER_RESULT]
      [SQL_CACHE | SQL_NO_CACHE] [SQL_CALC_FOUND_ROWS]
    select_expr [, select_expr ...]
    [FROM table_references
    [WHERE where_condition]
    [GROUP BY {col_name | expr | position}
      [ASC | DESC], ... [WITH ROLLUP]]
    [HAVING where_condition]
    [ORDER BY {col_name | expr | position}
      [ASC | DESC], ...]
    [LIMIT {[offset,] row_count | row_count OFFSET offset}]
    [PROCEDURE procedure_name(argument_list)]
    [INTO OUTFILE 'file_name'
        [CHARACTER SET charset_name]
        export_options
      | INTO DUMPFILE 'file_name'
      | INTO var_name [, var_name]]
    [FOR UPDATE | LOCK IN SHARE MODE]]

SELECT is used to retrieve rows selected from one or more tables, and
can include UNION statements and subqueries. See [HELP UNION], and
http://dev.mysql.com/doc/refman/5.5/en/subqueries.html.

The most commonly used clauses of SELECT statements are these:

o Each select_expr indicates a column that you want to retrieve. There
  must be at least one select_expr.

o table_references indicates the table or tables from which to retrieve
  rows. Its syntax is described in [HELP JOIN].

o The WHERE clause, if given, indicates the condition or conditions
  that rows must satisfy to be selected. where_condition is an
  expression that evaluates to true for each row to be selected. The
  statement selects all rows if there is no WHERE clause.

  In the WHERE expression, you can use any of the functions and
  operators that MySQL supports, except for aggregate (summary)
  functions. See
  http://dev.mysql.com/doc/refman/5.5/en/expressions.html, and
  http://dev.mysql.com/doc/refman/5.5/en/functions.html.

SELECT can also be used to retrieve rows computed without reference to
any table.

URL: http://dev.mysql.com/doc/refman/5.5/en/select.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/select.html</field>
	</row>
	<row>
		<field name="help_topic_id">358</field>
		<field name="name">COT</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
COT(X)

Returns the cotangent of X.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT COT(12);
        -&gt; -1.5726734063977
mysql&gt; SELECT COT(0);
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">359</field>
		<field name="name">SHOW CREATE EVENT</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW CREATE EVENT event_name

This statement displays the CREATE EVENT statement needed to re-create
a given event. It requires the EVENT privilege for the database from
which the event is to be shown. For example (using the same event
e_daily defined and then altered in [HELP SHOW EVENTS]):

URL: http://dev.mysql.com/doc/refman/5.5/en/show-create-event.html

</field>
		<field name="example">mysql&gt; SHOW CREATE EVENT test.e_daily\G
*************************** 1. row ***************************
               Event: e_daily
            sql_mode:
           time_zone: SYSTEM
        Create Event: CREATE EVENT `e_daily`
                        ON SCHEDULE EVERY 1 DAY
                        STARTS CURRENT_TIMESTAMP + INTERVAL 6 HOUR
                        ON COMPLETION NOT PRESERVE
                        ENABLE
                        COMMENT 'Saves total number of sessions then
                                clears the table each day'
                        DO BEGIN
                          INSERT INTO site_activity.totals (time, total)
                            SELECT CURRENT_TIMESTAMP, COUNT(*)
                            FROM site_activity.sessions;
                          DELETE FROM site_activity.sessions;
                        END
character_set_client: latin1
collation_connection: latin1_swedish_ci
  Database Collation: latin1_swedish_ci
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-create-event.html</field>
	</row>
	<row>
		<field name="help_topic_id">360</field>
		<field name="name">LOAD_FILE</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
LOAD_FILE(file_name)

Reads the file and returns the file contents as a string. To use this
function, the file must be located on the server host, you must specify
the full path name to the file, and you must have the FILE privilege.
The file must be readable by all and its size less than
max_allowed_packet bytes. If the secure_file_priv system variable is
set to a nonempty directory name, the file to be loaded must be located
in that directory.

If the file does not exist or cannot be read because one of the
preceding conditions is not satisfied, the function returns NULL.

The character_set_filesystem system variable controls interpretation of
file names that are given as literal strings.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; UPDATE t
            SET blob_col=LOAD_FILE('/tmp/picture')
            WHERE id=1;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">361</field>
		<field name="name">POINTFROMTEXT</field>
		<field name="help_category_id">3</field>
		<field name="description">PointFromText(wkt[,srid])

Constructs a POINT value using its WKT representation and SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">362</field>
		<field name="name">GROUP_CONCAT</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
GROUP_CONCAT(expr)

This function returns a string result with the concatenated non-NULL
values from a group. It returns NULL if there are no non-NULL values.
The full syntax is as follows:

GROUP_CONCAT([DISTINCT] expr [,expr ...]
             [ORDER BY {unsigned_integer | col_name | expr}
                 [ASC | DESC] [,col_name ...]]
             [SEPARATOR str_val])

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example">mysql&gt; SELECT student_name,
    -&gt;     GROUP_CONCAT(test_score)
    -&gt;     FROM student
    -&gt;     GROUP BY student_name;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">363</field>
		<field name="name">DATE_FORMAT</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
DATE_FORMAT(date,format)

Formats the date value according to the format string.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT DATE_FORMAT('2009-10-04 22:23:00', '%W %M %Y');
        -&gt; 'Sunday October 2009'
mysql&gt; SELECT DATE_FORMAT('2007-10-04 22:23:00', '%H:%i:%s');
        -&gt; '22:23:00'
mysql&gt; SELECT DATE_FORMAT('1900-10-04 22:23:00',
    -&gt;                 '%D %y %a %d %m %b %j');
        -&gt; '4th 00 Thu 04 10 Oct 277'
mysql&gt; SELECT DATE_FORMAT('1997-10-04 22:23:00',
    -&gt;                 '%H %k %I %r %T %S %w');
        -&gt; '22 22 10 10:23:00 PM 22:23:00 00 6'
mysql&gt; SELECT DATE_FORMAT('1999-01-01', '%X %V');
        -&gt; '1998 52'
mysql&gt; SELECT DATE_FORMAT('2006-06-00', '%d');
        -&gt; '00'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">364</field>
		<field name="name">BENCHMARK</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
BENCHMARK(count,expr)

The BENCHMARK() function executes the expression expr repeatedly count
times. It may be used to time how quickly MySQL processes the
expression. The result value is always 0. The intended use is from
within the mysql client, which reports query execution times:

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example">mysql&gt; SELECT BENCHMARK(1000000,ENCODE('hello','goodbye'));
+----------------------------------------------+
| BENCHMARK(1000000,ENCODE('hello','goodbye')) |
+----------------------------------------------+
|                                            0 |
+----------------------------------------------+
1 row in set (4.74 sec)
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">365</field>
		<field name="name">YEAR</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
YEAR(date)

Returns the year for date, in the range 1000 to 9999, or 0 for the
&quot;zero&quot; date.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT YEAR('1987-01-01');
        -&gt; 1987
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">366</field>
		<field name="name">SHOW ENGINE</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW ENGINE engine_name {STATUS | MUTEX}

SHOW ENGINE displays operational information about a storage engine.
The following statements currently are supported:

SHOW ENGINE INNODB STATUS
SHOW ENGINE INNODB MUTEX
SHOW ENGINE {NDB | NDBCLUSTER} STATUS
SHOW ENGINE PERFORMANCE_SCHEMA STATUS

SHOW ENGINE INNODB STATUS displays extensive information from the
standard InnoDB Monitor about the state of the InnoDB storage engine.
For information about the standard monitor and other InnoDB Monitors
that provide information about InnoDB processing, see
http://dev.mysql.com/doc/refman/5.5/en/innodb-monitors.html.

SHOW ENGINE INNODB MUTEX displays InnoDB mutex statistics. The
statement displays the following fields:

o Type

  Always InnoDB.

o Name

  The source file where the mutex is implemented, and the line number
  in the file where the mutex is created. The line number may change
  depending on your version of MySQL.

o Status

  The mutex status. This field displays several values if UNIV_DEBUG
  was defined at MySQL compilation time (for example, in include/univ.i
  in the InnoDB part of the MySQL source tree). If UNIV_DEBUG was not
  defined, the statement displays only the os_waits value. In the
  latter case (without UNIV_DEBUG), the information on which the output
  is based is insufficient to distinguish regular mutexes and mutexes
  that protect rw-locks (which permit multiple readers or a single
  writer). Consequently, the output may appear to contain multiple rows
  for the same mutex.

  o count indicates how many times the mutex was requested.

  o spin_waits indicates how many times the spinlock had to run.

  o spin_rounds indicates the number of spinlock rounds. (spin_rounds
    divided by spin_waits provides the average round count.)

  o os_waits indicates the number of operating system waits. This
    occurs when the spinlock did not work (the mutex was not locked
    during the spinlock and it was necessary to yield to the operating
    system and wait).

  o os_yields indicates the number of times a the thread trying to lock
    a mutex gave up its timeslice and yielded to the operating system
    (on the presumption that permitting other threads to run will free
    the mutex so that it can be locked).

  o os_wait_times indicates the amount of time (in ms) spent in
    operating system waits, if the timed_mutexes system variable is 1
    (ON). If timed_mutexes is 0 (OFF), timing is disabled, so
    os_wait_times is 0. timed_mutexes is off by default.

Information from this statement can be used to diagnose system
problems. For example, large values of spin_waits and spin_rounds may
indicate scalability problems.

Use SHOW ENGINE PERFORMANCE_SCHEMA STATUS to inspect the internal
operation of the Performance Schema code:

mysql&gt; SHOW ENGINE PERFORMANCE_SCHEMA STATUS\G
...
*************************** 3. row ***************************
  Type: performance_schema
  Name: events_waits_history.row_size
Status: 76
*************************** 4. row ***************************
  Type: performance_schema
  Name: events_waits_history.row_count
Status: 10000
*************************** 5. row ***************************
  Type: performance_schema
  Name: events_waits_history.memory
Status: 760000
...
*************************** 57. row ***************************
  Type: performance_schema
  Name: performance_schema.memory
Status: 26459600
...

This statement is intended to help the DBA understand the effects that
different Performance Schema options have on memory requirements.

Name values consist of two parts, which name an internal buffer and a
buffer attribute, respectively. Interpret buffer names as follows:

o An internal buffer that is not exposed as a table is named within
  parentheses. Examples: (pfs_cond_class).row_size,
  (pfs_mutex_class).memory.

o An internal buffer that is exposed as a table in the
  performance_schema database is named after the table, without
  parentheses. Examples: events_waits_history.row_size,
  mutex_instances.row_count.

o A value that applies to the Performance Schema as a whole begins with
  performance_schema. Example: performance_schema.memory.

Buffer attributes have these meanings:

o row_size is the size of the internal record used by the
  implementation, such as the size of a row in a table. row_size values
  cannot be changed.

o row_count is the number of internal records, such as the number of
  rows in a table. row_count values can be changed using Performance
  Schema configuration options.

o For a table, tbl_name.memory is the product of row_size and
  row_count. For the Performance Schema as a whole,
  performance_schema.memory is the sum of all the memory used (the sum
  of all other memory values).

In some cases, there is a direct relationship between a Performance
Schema configuration parameter and a SHOW ENGINE value. For example,
events_waits_history_long.row_count corresponds to
performance_schema_events_waits_history_long_size. In other cases, the
relationship is more complex. For example,
events_waits_history.row_count corresponds to
performance_schema_events_waits_history_size (the number of rows per
thread) multiplied by performance_schema_max_thread_instances ( the
number of threads).

If the server has the NDBCLUSTER storage engine enabled, SHOW ENGINE
NDB STATUS displays cluster status information such as the number of
connected data nodes, the cluster connectstring, and cluster binlog
epochs, as well as counts of various Cluster API objects created by the
MySQL Server when connected to the cluster. Sample output from this
statement is shown here:

mysql&gt; SHOW ENGINE NDB STATUS;
+------------+-----------------------+--------------------------------------------------+
| Type       | Name                  | Status                                           |
+------------+-----------------------+--------------------------------------------------+
| ndbcluster | connection            | cluster_node_id=7,
  connected_host=192.168.0.103, connected_port=1186, number_of_data_nodes=4,
  number_of_ready_data_nodes=3, connect_count=0                                         |
| ndbcluster | NdbTransaction        | created=6, free=0, sizeof=212                    |
| ndbcluster | NdbOperation          | created=8, free=8, sizeof=660                    |
| ndbcluster | NdbIndexScanOperation | created=1, free=1, sizeof=744                    |
| ndbcluster | NdbIndexOperation     | created=0, free=0, sizeof=664                    |
| ndbcluster | NdbRecAttr            | created=1285, free=1285, sizeof=60               |
| ndbcluster | NdbApiSignal          | created=16, free=16, sizeof=136                  |
| ndbcluster | NdbLabel              | created=0, free=0, sizeof=196                    |
| ndbcluster | NdbBranch             | created=0, free=0, sizeof=24                     |
| ndbcluster | NdbSubroutine         | created=0, free=0, sizeof=68                     |
| ndbcluster | NdbCall               | created=0, free=0, sizeof=16                     |
| ndbcluster | NdbBlob               | created=1, free=1, sizeof=264                    |
| ndbcluster | NdbReceiver           | created=4, free=0, sizeof=68                     |
| ndbcluster | binlog                | latest_epoch=155467, latest_trans_epoch=148126,
  latest_received_binlog_epoch=0, latest_handled_binlog_epoch=0,
  latest_applied_binlog_epoch=0                                                         |
+------------+-----------------------+--------------------------------------------------+

The rows with connection and binlog in the Name column were added to
the output of this statement in MySQL 5.1. The Status column in each of
these rows provides information about the MySQL server's connection to
the cluster and about the cluster binary log's status, respectively.
The Status information is in the form of comma-delimited set of
name/value pairs.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-engine.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-engine.html</field>
	</row>
	<row>
		<field name="help_topic_id">367</field>
		<field name="name">NAME_CONST</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
NAME_CONST(name,value)

Returns the given value. When used to produce a result set column,
NAME_CONST() causes the column to have the given name. The arguments
should be constants.

mysql&gt; SELECT NAME_CONST('myname', 14);
+--------+
| myname |
+--------+
|     14 |
+--------+

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">368</field>
		<field name="name">RELEASE_LOCK</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
RELEASE_LOCK(str)

Releases the lock named by the string str that was obtained with
GET_LOCK(). Returns 1 if the lock was released, 0 if the lock was not
established by this thread (in which case the lock is not released),
and NULL if the named lock did not exist. The lock does not exist if it
was never obtained by a call to GET_LOCK() or if it has previously been
released.

The DO statement is convenient to use with RELEASE_LOCK(). See [HELP
DO].

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">369</field>
		<field name="name">IS NULL</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
IS NULL

Tests whether a value is NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT 1 IS NULL, 0 IS NULL, NULL IS NULL;
        -&gt; 0, 0, 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">370</field>
		<field name="name">CONVERT_TZ</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
CONVERT_TZ(dt,from_tz,to_tz)

CONVERT_TZ() converts a datetime value dt from the time zone given by
from_tz to the time zone given by to_tz and returns the resulting
value. Time zones are specified as described in
http://dev.mysql.com/doc/refman/5.5/en/time-zone-support.html. This
function returns NULL if the arguments are invalid.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT CONVERT_TZ('2004-01-01 12:00:00','GMT','MET');
        -&gt; '2004-01-01 13:00:00'
mysql&gt; SELECT CONVERT_TZ('2004-01-01 12:00:00','+00:00','+10:00');
        -&gt; '2004-01-01 22:00:00'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">371</field>
		<field name="name">TIME_TO_SEC</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
TIME_TO_SEC(time)

Returns the time argument, converted to seconds.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT TIME_TO_SEC('22:23:00');
        -&gt; 80580
mysql&gt; SELECT TIME_TO_SEC('00:39:38');
        -&gt; 2378
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">372</field>
		<field name="name">WEEKDAY</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
WEEKDAY(date)

Returns the weekday index for date (0 = Monday, 1 = Tuesday, ... 6 =
Sunday).

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT WEEKDAY('2008-02-03 22:23:00');
        -&gt; 6
mysql&gt; SELECT WEEKDAY('2007-11-06');
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">373</field>
		<field name="name">EXPORT_SET</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
EXPORT_SET(bits,on,off[,separator[,number_of_bits]])

Returns a string such that for every bit set in the value bits, you get
an on string and for every bit not set in the value, you get an off
string. Bits in bits are examined from right to left (from low-order to
high-order bits). Strings are added to the result from left to right,
separated by the separator string (the default being the comma
character &quot;,&quot;). The number of bits examined is given by number_of_bits,
which has a default of 64 if not specified. number_of_bits is silently
clipped to 64 if larger than 64. It is treated as an unsigned integer,
so a value of -1 is effectively the same as 64.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT EXPORT_SET(5,'Y','N',',',4);
        -&gt; 'Y,N,Y,N'
mysql&gt; SELECT EXPORT_SET(6,'1','0',',',10);
        -&gt; '0,1,1,0,0,0,0,0,0,0'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">374</field>
		<field name="name">ALTER SERVER</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
ALTER SERVER  server_name
    OPTIONS (option [, option] ...)

Alters the server information for server_name, adjusting any of the
options permitted in the CREATE SERVER statement. The corresponding
fields in the mysql.servers table are updated accordingly. This
statement requires the SUPER privilege.

URL: http://dev.mysql.com/doc/refman/5.5/en/alter-server.html

</field>
		<field name="example">ALTER SERVER s OPTIONS (USER 'sally');
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/alter-server.html</field>
	</row>
	<row>
		<field name="help_topic_id">375</field>
		<field name="name">RESIGNAL</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
RESIGNAL [condition_value]
    [SET signal_information_item
    [, signal_information_item] ...]

condition_value:
    SQLSTATE [VALUE] sqlstate_value
  | condition_name

signal_information_item:
    condition_information_item_name = simple_value_specification

condition_information_item_name:
    CLASS_ORIGIN
  | SUBCLASS_ORIGIN
  | MESSAGE_TEXT
  | MYSQL_ERRNO
  | CONSTRAINT_CATALOG
  | CONSTRAINT_SCHEMA
  | CONSTRAINT_NAME
  | CATALOG_NAME
  | SCHEMA_NAME
  | TABLE_NAME
  | COLUMN_NAME
  | CURSOR_NAME

condition_name, simple_value_specification:
    (see following discussion)

RESIGNAL passes on the error condition information that is available
during execution of a condition handler within a compound statement
inside a stored procedure or function, trigger, or event. RESIGNAL may
change some or all information before passing it on. RESIGNAL is
related to SIGNAL, but instead of originating a condition as SIGNAL
does, RESIGNAL relays existing condition information, possibly after
modifying it.

RESIGNAL makes it possible to both handle an error and return the error
information. Otherwise, by executing an SQL statement within the
handler, information that caused the handler's activation is destroyed.
RESIGNAL also can make some procedures shorter if a given handler can
handle part of a situation, then pass the condition &quot;up the line&quot; to
another handler.

No special privileges are required to execute the RESIGNAL statement.

For condition_value and signal_information_item, the definitions and
rules are the same for RESIGNAL as for SIGNAL (see [HELP SIGNAL]).

The RESIGNAL statement takes condition_value and SET clauses, both of
which are optional. This leads to several possible uses:

o RESIGNAL alone:

RESIGNAL;

o RESIGNAL with new signal information:

RESIGNAL SET signal_information_item [, signal_information_item] ...;

o RESIGNAL with a condition value and possibly new signal information:

RESIGNAL condition_value
    [SET signal_information_item [, signal_information_item] ...];

URL: http://dev.mysql.com/doc/refman/5.5/en/resignal.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/resignal.html</field>
	</row>
	<row>
		<field name="help_topic_id">376</field>
		<field name="name">TIME FUNCTION</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
TIME(expr)

Extracts the time part of the time or datetime expression expr and
returns it as a string.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT TIME('2003-12-31 01:02:03');
        -&gt; '01:02:03'
mysql&gt; SELECT TIME('2003-12-31 01:02:03.000123');
        -&gt; '01:02:03.000123'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">377</field>
		<field name="name">DATE_ADD</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
DATE_ADD(date,INTERVAL expr unit), DATE_SUB(date,INTERVAL expr unit)

These functions perform date arithmetic. The date argument specifies
the starting date or datetime value. expr is an expression specifying
the interval value to be added or subtracted from the starting date.
expr is a string; it may start with a &quot;-&quot; for negative intervals. unit
is a keyword indicating the units in which the expression should be
interpreted.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT '2008-12-31 23:59:59' + INTERVAL 1 SECOND;
        -&gt; '2009-01-01 00:00:00'
mysql&gt; SELECT INTERVAL 1 DAY + '2008-12-31';
        -&gt; '2009-01-01'
mysql&gt; SELECT '2005-01-01' - INTERVAL 1 SECOND;
        -&gt; '2004-12-31 23:59:59'
mysql&gt; SELECT DATE_ADD('2000-12-31 23:59:59',
    -&gt;                 INTERVAL 1 SECOND);
        -&gt; '2001-01-01 00:00:00'
mysql&gt; SELECT DATE_ADD('2010-12-31 23:59:59',
    -&gt;                 INTERVAL 1 DAY);
        -&gt; '2011-01-01 23:59:59'
mysql&gt; SELECT DATE_ADD('2100-12-31 23:59:59',
    -&gt;                 INTERVAL '1:1' MINUTE_SECOND);
        -&gt; '2101-01-01 00:01:00'
mysql&gt; SELECT DATE_SUB('2005-01-01 00:00:00',
    -&gt;                 INTERVAL '1 1:1:1' DAY_SECOND);
        -&gt; '2004-12-30 22:58:59'
mysql&gt; SELECT DATE_ADD('1900-01-01 00:00:00',
    -&gt;                 INTERVAL '-1 10' DAY_HOUR);
        -&gt; '1899-12-30 14:00:00'
mysql&gt; SELECT DATE_SUB('1998-01-02', INTERVAL 31 DAY);
        -&gt; '1997-12-02'
mysql&gt; SELECT DATE_ADD('1992-12-31 23:59:59.000002',
    -&gt;            INTERVAL '1.999999' SECOND_MICROSECOND);
        -&gt; '1993-01-01 00:00:01.000001'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">378</field>
		<field name="name">CAST</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
CAST(expr AS type)

The CAST() function takes an expression of any type and produces a
result value of a specified type, similar to CONVERT(). See the
description of CONVERT() for more information.

URL: http://dev.mysql.com/doc/refman/5.5/en/cast-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/cast-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">379</field>
		<field name="name">SOUNDS LIKE</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
expr1 SOUNDS LIKE expr2

This is the same as SOUNDEX(expr1) = SOUNDEX(expr2).

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">380</field>
		<field name="name">PERIOD_DIFF</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
PERIOD_DIFF(P1,P2)

Returns the number of months between periods P1 and P2. P1 and P2
should be in the format YYMM or YYYYMM. Note that the period arguments
P1 and P2 are not date values.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT PERIOD_DIFF(200802,200703);
        -&gt; 11
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">381</field>
		<field name="name">LIKE</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
expr LIKE pat [ESCAPE 'escape_char']

Pattern matching using SQL simple regular expression comparison.
Returns 1 (TRUE) or 0 (FALSE). If either expr or pat is NULL, the
result is NULL.

The pattern need not be a literal string. For example, it can be
specified as a string expression or table column.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-comparison-functions.html

</field>
		<field name="example">mysql&gt; SELECT 'David!' LIKE 'David_';
        -&gt; 1
mysql&gt; SELECT 'David!' LIKE '%D%v%';
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-comparison-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">382</field>
		<field name="name">MULTIPOINT</field>
		<field name="help_category_id">25</field>
		<field name="description">MultiPoint(pt1,pt2,...)

Constructs a MultiPoint value using Point or WKB Point arguments.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">383</field>
		<field name="name">&gt;&gt;</field>
		<field name="help_category_id">20</field>
		<field name="description">Syntax:
&gt;&gt;

Shifts a longlong (BIGINT) number to the right.

URL: http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html

</field>
		<field name="example">mysql&gt; SELECT 4 &gt;&gt; 2;
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">384</field>
		<field name="name">FETCH</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
FETCH [[NEXT] FROM] cursor_name INTO var_name [, var_name] ...

This statement fetches the next row for the SELECT statement associated
with the specified cursor (which must be open), and advances the cursor
pointer. If a row exists, the fetched columns are stored in the named
variables. The number of columns retrieved by the SELECT statement must
match the number of output variables specified in the FETCH statement.

If no more rows are available, a No Data condition occurs with SQLSTATE
value '02000'. To detect this condition, you can set up a handler for
it (or for a NOT FOUND condition). For an example, see
http://dev.mysql.com/doc/refman/5.5/en/cursors.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/fetch.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/fetch.html</field>
	</row>
	<row>
		<field name="help_topic_id">385</field>
		<field name="name">AVG</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
AVG([DISTINCT] expr)

Returns the average value of expr. The DISTINCT option can be used to
return the average of the distinct values of expr.

AVG() returns NULL if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example">mysql&gt; SELECT student_name, AVG(test_score)
    -&gt;        FROM student
    -&gt;        GROUP BY student_name;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">386</field>
		<field name="name">TRUE FALSE</field>
		<field name="help_category_id">30</field>
		<field name="description">The constants TRUE and FALSE evaluate to 1 and 0, respectively. The
constant names can be written in any lettercase.

mysql&gt; SELECT TRUE, true, FALSE, false;
        -&gt; 1, 1, 0, 0

URL: http://dev.mysql.com/doc/refman/5.5/en/boolean-literals.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/boolean-literals.html</field>
	</row>
	<row>
		<field name="help_topic_id">387</field>
		<field name="name">MBRWITHIN</field>
		<field name="help_category_id">6</field>
		<field name="description">MBRWithin(g1,g2)

Returns 1 or 0 to indicate whether the Minimum Bounding Rectangle of g1
is within the Minimum Bounding Rectangle of g2. This tests the opposite
relationship as MBRContains().

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example">mysql&gt; SET @g1 = GeomFromText('Polygon((0 0,0 3,3 3,3 0,0 0))');
mysql&gt; SET @g2 = GeomFromText('Polygon((0 0,0 5,5 5,5 0,0 0))');
mysql&gt; SELECT MBRWithin(@g1,@g2), MBRWithin(@g2,@g1);
+--------------------+--------------------+
| MBRWithin(@g1,@g2) | MBRWithin(@g2,@g1) |
+--------------------+--------------------+
|                  1 |                  0 |
+--------------------+--------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">388</field>
		<field name="name">SESSION_USER</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
SESSION_USER()

SESSION_USER() is a synonym for USER().

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">389</field>
		<field name="name">IN</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
expr IN (value,...)

Returns 1 if expr is equal to any of the values in the IN list, else
returns 0. If all values are constants, they are evaluated according to
the type of expr and sorted. The search for the item then is done using
a binary search. This means IN is very quick if the IN value list
consists entirely of constants. Otherwise, type conversion takes place
according to the rules described in
http://dev.mysql.com/doc/refman/5.5/en/type-conversion.html, but
applied to all the arguments.

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT 2 IN (0,3,5,7);
        -&gt; 0
mysql&gt; SELECT 'wefwf' IN ('wee','wefwf','weg');
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">390</field>
		<field name="name">QUOTE</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
QUOTE(str)

Quotes a string to produce a result that can be used as a properly
escaped data value in an SQL statement. The string is returned enclosed
by single quotation marks and with each instance of backslash (&quot;\&quot;),
single quote (&quot;'&quot;), ASCII NUL, and Control+Z preceded by a backslash.
If the argument is NULL, the return value is the word &quot;NULL&quot; without
enclosing single quotation marks.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT QUOTE('Don\'t!');
        -&gt; 'Don\'t!'
mysql&gt; SELECT QUOTE(NULL);
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">391</field>
		<field name="name">HELP COMMAND</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
mysql&gt; help search_string

If you provide an argument to the help command, mysql uses it as a
search string to access server-side help from the contents of the MySQL
Reference Manual. The proper operation of this command requires that
the help tables in the mysql database be initialized with help topic
information (see
http://dev.mysql.com/doc/refman/5.5/en/server-side-help-support.html).

If there is no match for the search string, the search fails:

mysql&gt; help me

Nothing found
Please try to run 'help contents' for a list of all accessible topics

Use help contents to see a list of the help categories:

mysql&gt; help contents
You asked for help about help category: &quot;Contents&quot;
For more information, type 'help &lt;item&gt;', where &lt;item&gt; is one of the
following categories:
   Account Management
   Administration
   Data Definition
   Data Manipulation
   Data Types
   Functions
   Functions and Modifiers for Use with GROUP BY
   Geographic Features
   Language Structure
   Plugins
   Storage Engines
   Stored Routines
   Table Maintenance
   Transactions
   Triggers

If the search string matches multiple items, mysql shows a list of
matching topics:

mysql&gt; help logs
Many help items for your request exist.
To make a more specific request, please type 'help &lt;item&gt;',
where &lt;item&gt; is one of the following topics:
   SHOW
   SHOW BINARY LOGS
   SHOW ENGINE
   SHOW LOGS

Use a topic as the search string to see the help entry for that topic:

mysql&gt; help show binary logs
Name: 'SHOW BINARY LOGS'
Description:
Syntax:
SHOW BINARY LOGS
SHOW MASTER LOGS

Lists the binary log files on the server. This statement is used as
part of the procedure described in [purge-binary-logs], that shows how
to determine which logs can be purged.

mysql&gt; SHOW BINARY LOGS;
+---------------+-----------+
| Log_name      | File_size |
+---------------+-----------+
| binlog.000015 |    724935 |
| binlog.000016 |    733481 |
+---------------+-----------+

URL: http://dev.mysql.com/doc/refman/5.5/en/mysql-server-side-help.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mysql-server-side-help.html</field>
	</row>
	<row>
		<field name="help_topic_id">392</field>
		<field name="name">QUARTER</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
QUARTER(date)

Returns the quarter of the year for date, in the range 1 to 4.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT QUARTER('2008-04-01');
        -&gt; 2
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">393</field>
		<field name="name">POSITION</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
POSITION(substr IN str)

POSITION(substr IN str) is a synonym for LOCATE(substr,str).

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">394</field>
		<field name="name">SHOW CREATE FUNCTION</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW CREATE FUNCTION func_name

This statement is similar to SHOW CREATE PROCEDURE but for stored
functions. See [HELP SHOW CREATE PROCEDURE].

URL: http://dev.mysql.com/doc/refman/5.5/en/show-create-function.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-create-function.html</field>
	</row>
	<row>
		<field name="help_topic_id">395</field>
		<field name="name">IS_USED_LOCK</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
IS_USED_LOCK(str)

Checks whether the lock named str is in use (that is, locked). If so,
it returns the connection identifier of the client that holds the lock.
Otherwise, it returns NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">396</field>
		<field name="name">POLYFROMTEXT</field>
		<field name="help_category_id">3</field>
		<field name="description">PolyFromText(wkt[,srid]), PolygonFromText(wkt[,srid])

Constructs a POLYGON value using its WKT representation and SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">397</field>
		<field name="name">DES_ENCRYPT</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
DES_ENCRYPT(str[,{key_num|key_str}])

Encrypts the string with the given key using the Triple-DES algorithm.

This function works only if MySQL has been configured with SSL support.
See http://dev.mysql.com/doc/refman/5.5/en/ssl-connections.html.

The encryption key to use is chosen based on the second argument to
DES_ENCRYPT(), if one was given. With no argument, the first key from
the DES key file is used. With a key_num argument, the given key number
(0 to 9) from the DES key file is used. With a key_str argument, the
given key string is used to encrypt str.

The key file can be specified with the --des-key-file server option.

The return string is a binary string where the first character is
CHAR(128 | key_num). If an error occurs, DES_ENCRYPT() returns NULL.

The 128 is added to make it easier to recognize an encrypted key. If
you use a string key, key_num is 127.

The string length for the result is given by this formula:

new_len = orig_len + (8 - (orig_len % 8)) + 1

Each line in the DES key file has the following format:

key_num des_key_str

Each key_num value must be a number in the range from 0 to 9. Lines in
the file may be in any order. des_key_str is the string that is used to
encrypt the message. There should be at least one space between the
number and the key. The first key is the default key that is used if
you do not specify any key argument to DES_ENCRYPT().

You can tell MySQL to read new key values from the key file with the
FLUSH DES_KEY_FILE statement. This requires the RELOAD privilege.

One benefit of having a set of default keys is that it gives
applications a way to check for the existence of encrypted column
values, without giving the end user the right to decrypt those values.

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example">mysql&gt; SELECT customer_address FROM customer_table 
     &gt; WHERE crypted_credit_card = DES_ENCRYPT('credit_card_number');
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">398</field>
		<field name="name">CEIL</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
CEIL(X)

CEIL() is a synonym for CEILING().

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">399</field>
		<field name="name">LENGTH</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
LENGTH(str)

Returns the length of the string str, measured in bytes. A multi-byte
character counts as multiple bytes. This means that for a string
containing five 2-byte characters, LENGTH() returns 10, whereas
CHAR_LENGTH() returns 5.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT LENGTH('text');
        -&gt; 4
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">400</field>
		<field name="name">STR_TO_DATE</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
STR_TO_DATE(str,format)

This is the inverse of the DATE_FORMAT() function. It takes a string
str and a format string format. STR_TO_DATE() returns a DATETIME value
if the format string contains both date and time parts, or a DATE or
TIME value if the string contains only date or time parts. If the date,
time, or datetime value extracted from str is illegal, STR_TO_DATE()
returns NULL and produces a warning.

The server scans str attempting to match format to it. The format
string can contain literal characters and format specifiers beginning
with %. Literal characters in format must match literally in str.
Format specifiers in format must match a date or time part in str. For
the specifiers that can be used in format, see the DATE_FORMAT()
function description.

mysql&gt; SELECT STR_TO_DATE('01,5,2013','%d,%m,%Y');
        -&gt; '2013-05-01'
mysql&gt; SELECT STR_TO_DATE('May 1, 2013','%M %d,%Y');
        -&gt; '2013-05-01'

Scanning starts at the beginning of str and fails if format is found
not to match. Extra characters at the end of str are ignored.

mysql&gt; SELECT STR_TO_DATE('a09:30:17','a%h:%i:%s');
        -&gt; '09:30:17'
mysql&gt; SELECT STR_TO_DATE('a09:30:17','%h:%i:%s');
        -&gt; NULL
mysql&gt; SELECT STR_TO_DATE('09:30:17a','%h:%i:%s');
        -&gt; '09:30:17'

Unspecified date or time parts have a value of 0, so incompletely
specified values in str produce a result with some or all parts set to
0:

mysql&gt; SELECT STR_TO_DATE('abc','abc');
        -&gt; '0000-00-00'
mysql&gt; SELECT STR_TO_DATE('9','%m');
        -&gt; '0000-09-00'
mysql&gt; SELECT STR_TO_DATE('9','%s');
        -&gt; '00:00:09'

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">401</field>
		<field name="name">Y</field>
		<field name="help_category_id">11</field>
		<field name="description">Y(p)

Returns the Y-coordinate value for the Point object p as a
double-precision number.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SELECT Y(POINT(56.7, 53.34));
+-----------------------+
| Y(POINT(56.7, 53.34)) |
+-----------------------+
|                 53.34 |
+-----------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">402</field>
		<field name="name">CHECKSUM TABLE</field>
		<field name="help_category_id">21</field>
		<field name="description">Syntax:
CHECKSUM TABLE tbl_name [, tbl_name] ... [ QUICK | EXTENDED ]

CHECKSUM TABLE reports a table checksum. This statement requires the
SELECT privilege for the table.

With QUICK, the live table checksum is reported if it is available, or
NULL otherwise. This is very fast. A live checksum is enabled by
specifying the CHECKSUM=1 table option when you create the table;
currently, this is supported only for MyISAM tables. See [HELP CREATE
TABLE].

With EXTENDED, the entire table is read row by row and the checksum is
calculated. This can be very slow for large tables.

If neither QUICK nor EXTENDED is specified, MySQL returns a live
checksum if the table storage engine supports it and scans the table
otherwise.

For a nonexistent table, CHECKSUM TABLE returns NULL and generates a
warning.

In MySQL 5.5, CHECKSUM TABLE returns 0 for partitioned tables unless
you include the EXTENDED option. This issue is resolved in MySQL 5.6.
(Bug #11933226, Bug #60681)

The checksum value depends on the table row format. If the row format
changes, the checksum also changes. For example, the storage format for
VARCHAR changed between MySQL 4.1 and 5.0, so if a 4.1 table is
upgraded to MySQL 5.0, the checksum value may change.

URL: http://dev.mysql.com/doc/refman/5.5/en/checksum-table.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/checksum-table.html</field>
	</row>
	<row>
		<field name="help_topic_id">403</field>
		<field name="name">NUMINTERIORRINGS</field>
		<field name="help_category_id">2</field>
		<field name="description">NumInteriorRings(poly)

Returns the number of interior rings in the Polygon value poly.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SET @poly =
    -&gt; 'Polygon((0 0,0 3,3 3,3 0,0 0),(1 1,1 2,2 2,2 1,1 1))';
mysql&gt; SELECT NumInteriorRings(GeomFromText(@poly));
+---------------------------------------+
| NumInteriorRings(GeomFromText(@poly)) |
+---------------------------------------+
|                                     1 |
+---------------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">404</field>
		<field name="name">INTERIORRINGN</field>
		<field name="help_category_id">2</field>
		<field name="description">InteriorRingN(poly,N)

Returns the N-th interior ring for the Polygon value poly as a
LineString. Rings are numbered beginning with 1.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SET @poly =
    -&gt; 'Polygon((0 0,0 3,3 3,3 0,0 0),(1 1,1 2,2 2,2 1,1 1))';
mysql&gt; SELECT AsText(InteriorRingN(GeomFromText(@poly),1));
+----------------------------------------------+
| AsText(InteriorRingN(GeomFromText(@poly),1)) |
+----------------------------------------------+
| LINESTRING(1 1,1 2,2 2,2 1,1 1)              |
+----------------------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">405</field>
		<field name="name">UTC_TIME</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
UTC_TIME, UTC_TIME()

Returns the current UTC time as a value in 'HH:MM:SS' or HHMMSS.uuuuuu
format, depending on whether the function is used in a string or
numeric context.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT UTC_TIME(), UTC_TIME() + 0;
        -&gt; '18:07:53', 180753.000000
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">406</field>
		<field name="name">DROP FUNCTION</field>
		<field name="help_category_id">40</field>
		<field name="description">The DROP FUNCTION statement is used to drop stored functions and
user-defined functions (UDFs):

o For information about dropping stored functions, see [HELP DROP
  PROCEDURE].

o For information about dropping user-defined functions, see [HELP DROP
  FUNCTION UDF].

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-function.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-function.html</field>
	</row>
	<row>
		<field name="help_topic_id">407</field>
		<field name="name">ALTER EVENT</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
ALTER
    [DEFINER = { user | CURRENT_USER }]
    EVENT event_name
    [ON SCHEDULE schedule]
    [ON COMPLETION [NOT] PRESERVE]
    [RENAME TO new_event_name]
    [ENABLE | DISABLE | DISABLE ON SLAVE]
    [COMMENT 'comment']
    [DO event_body]

The ALTER EVENT statement changes one or more of the characteristics of
an existing event without the need to drop and recreate it. The syntax
for each of the DEFINER, ON SCHEDULE, ON COMPLETION, COMMENT, ENABLE /
DISABLE, and DO clauses is exactly the same as when used with CREATE
EVENT. (See [HELP CREATE EVENT].)

Any user can alter an event defined on a database for which that user
has the EVENT privilege. When a user executes a successful ALTER EVENT
statement, that user becomes the definer for the affected event.

ALTER EVENT works only with an existing event:

mysql&gt; ALTER EVENT no_such_event 
     &gt;     ON SCHEDULE 
     &gt;       EVERY '2:3' DAY_HOUR;
ERROR 1517 (HY000): Unknown event 'no_such_event'

URL: http://dev.mysql.com/doc/refman/5.5/en/alter-event.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/alter-event.html</field>
	</row>
	<row>
		<field name="help_topic_id">408</field>
		<field name="name">STDDEV</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
STDDEV(expr)

Returns the population standard deviation of expr. This function is
provided for compatibility with Oracle. The standard SQL function
STDDEV_POP() can be used instead.

This function returns NULL if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">409</field>
		<field name="name">DATE_SUB</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
DATE_SUB(date,INTERVAL expr unit)

See the description for DATE_ADD().

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">410</field>
		<field name="name">PERIOD_ADD</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
PERIOD_ADD(P,N)

Adds N months to period P (in the format YYMM or YYYYMM). Returns a
value in the format YYYYMM. Note that the period argument P is not a
date value.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT PERIOD_ADD(200801,2);
        -&gt; 200803
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">411</field>
		<field name="name">|</field>
		<field name="help_category_id">20</field>
		<field name="description">Syntax:
|

Bitwise OR:

URL: http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html

</field>
		<field name="example">mysql&gt; SELECT 29 | 15;
        -&gt; 31
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">412</field>
		<field name="name">GEOMFROMTEXT</field>
		<field name="help_category_id">3</field>
		<field name="description">GeomFromText(wkt[,srid]), GeometryFromText(wkt[,srid])

Constructs a geometry value of any type using its WKT representation
and SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">413</field>
		<field name="name">UUID_SHORT</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
UUID_SHORT()

Returns a &quot;short&quot; universal identifier as a 64-bit unsigned integer
(rather than a string-form 128-bit identifier as returned by the UUID()
function).

The value of UUID_SHORT() is guaranteed to be unique if the following
conditions hold:

o The server_id of the current host is unique among your set of master
  and slave servers

o server_id is between 0 and 255

o You do not set back your system time for your server between mysqld
  restarts

o You do not invoke UUID_SHORT() on average more than 16 million times
  per second between mysqld restarts

The UUID_SHORT() return value is constructed this way:

  (server_id &amp; 255) &lt;&lt; 56
+ (server_startup_time_in_seconds &lt;&lt; 24)
+ incremented_variable++;

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example">mysql&gt; SELECT UUID_SHORT();
        -&gt; 92395783831158784
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">414</field>
		<field name="name">RIGHT</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
RIGHT(str,len)

Returns the rightmost len characters from the string str, or NULL if
any argument is NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT RIGHT('foobarbar', 4);
        -&gt; 'rbar'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">415</field>
		<field name="name">DATEDIFF</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
DATEDIFF(expr1,expr2)

DATEDIFF() returns expr1 - expr2 expressed as a value in days from one
date to the other. expr1 and expr2 are date or date-and-time
expressions. Only the date parts of the values are used in the
calculation.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT DATEDIFF('2007-12-31 23:59:59','2007-12-30');
        -&gt; 1
mysql&gt; SELECT DATEDIFF('2010-11-30 23:59:59','2010-12-31');
        -&gt; -31
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">416</field>
		<field name="name">DROP TABLESPACE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
DROP TABLESPACE tablespace_name
    ENGINE [=] engine_name

This statement drops a tablespace that was previously created using
CREATE TABLESPACE (see [HELP CREATE TABLESPACE]).

*Important*: The tablespace to be dropped must not contain any data
files; in other words, before you can drop a tablespace, you must first
drop each of its data files using ALTER TABLESPACE ... DROP DATAFILE
(see [HELP ALTER TABLESPACE]).

The ENGINE clause (required) specifies the storage engine used by the
tablespace. Currently, the only accepted values for engine_name are NDB
and NDBCLUSTER.

DROP TABLESPACE is useful only with Disk Data storage for MySQL
Cluster. See
http://dev.mysql.com/doc/refman/5.5/en/mysql-cluster-disk-data.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-tablespace.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-tablespace.html</field>
	</row>
	<row>
		<field name="help_topic_id">417</field>
		<field name="name">DROP PROCEDURE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
DROP {PROCEDURE | FUNCTION} [IF EXISTS] sp_name

This statement is used to drop a stored procedure or function. That is,
the specified routine is removed from the server. You must have the
ALTER ROUTINE privilege for the routine. (If the
automatic_sp_privileges system variable is enabled, that privilege and
EXECUTE are granted automatically to the routine creator when the
routine is created and dropped from the creator when the routine is
dropped. See
http://dev.mysql.com/doc/refman/5.5/en/stored-routines-privileges.html.
)

The IF EXISTS clause is a MySQL extension. It prevents an error from
occurring if the procedure or function does not exist. A warning is
produced that can be viewed with SHOW WARNINGS.

URL: http://dev.mysql.com/doc/refman/5.5/en/drop-procedure.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/drop-procedure.html</field>
	</row>
	<row>
		<field name="help_topic_id">418</field>
		<field name="name">CHECK TABLE</field>
		<field name="help_category_id">21</field>
		<field name="description">Syntax:
CHECK TABLE tbl_name [, tbl_name] ... [option] ...

option = {FOR UPGRADE | QUICK | FAST | MEDIUM | EXTENDED | CHANGED}

CHECK TABLE checks a table or tables for errors. CHECK TABLE works for
InnoDB, MyISAM, ARCHIVE, and CSV tables. For MyISAM tables, the key
statistics are updated as well.

To check a table, you must have some privilege for it.

CHECK TABLE can also check views for problems, such as tables that are
referenced in the view definition that no longer exist.

CHECK TABLE is supported for partitioned tables, and you can use ALTER
TABLE ... CHECK PARTITION to check one or more partitions; for more
information, see [HELP ALTER TABLE], and
http://dev.mysql.com/doc/refman/5.5/en/partitioning-maintenance.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/check-table.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/check-table.html</field>
	</row>
	<row>
		<field name="help_topic_id">419</field>
		<field name="name">BIN</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
BIN(N)

Returns a string representation of the binary value of N, where N is a
longlong (BIGINT) number. This is equivalent to CONV(N,10,2). Returns
NULL if N is NULL.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT BIN(12);
        -&gt; '1100'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">420</field>
		<field name="name">INSTALL PLUGIN</field>
		<field name="help_category_id">5</field>
		<field name="description">Syntax:
INSTALL PLUGIN plugin_name SONAME 'shared_library_name'

This statement installs a server plugin. It requires the INSERT
privilege for the mysql.plugin table.

plugin_name is the name of the plugin as defined in the plugin
descriptor structure contained in the library file (see
http://dev.mysql.com/doc/refman/5.5/en/plugin-data-structures.html).
Plugin names are not case sensitive. For maximal compatibility, plugin
names should be limited to ASCII letters, digits, and underscore
because they are used in C source files, shell command lines, M4 and
Bourne shell scripts, and SQL environments.

shared_library_name is the name of the shared library that contains the
plugin code. The name includes the file name extension (for example,
libmyplugin.so, libmyplugin.dll, or libmyplugin.dylib).

The shared library must be located in the plugin directory (the
directory named by the plugin_dir system variable). The library must be
in the plugin directory itself, not in a subdirectory. By default,
plugin_dir is the plugin directory under the directory named by the
pkglibdir configuration variable, but it can be changed by setting the
value of plugin_dir at server startup. For example, set its value in a
my.cnf file:

[mysqld]
plugin_dir=/path/to/plugin/directory

If the value of plugin_dir is a relative path name, it is taken to be
relative to the MySQL base directory (the value of the basedir system
variable).

INSTALL PLUGIN loads and initializes the plugin code to make the plugin
available for use. A plugin is initialized by executing its
initialization function, which handles any setup that the plugin must
perform before it can be used. When the server shuts down, it executes
the deinitialization function for each plugin that is loaded so that
the plugin has a change to perform any final cleanup.

INSTALL PLUGIN also registers the plugin by adding a line that
indicates the plugin name and library file name to the mysql.plugin
table. At server startup, the server loads and initializes any plugin
that is listed in the mysql.plugin table. This means that a plugin is
installed with INSTALL PLUGIN only once, not every time the server
starts. Plugin loading at startup does not occur if the server is
started with the --skip-grant-tables option.

A plugin library can contain multiple plugins. For each of them to be
installed, use a separate INSTALL PLUGIN statement. Each statement
names a different plugin, but all of them specify the same library
name.

URL: http://dev.mysql.com/doc/refman/5.5/en/install-plugin.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/install-plugin.html</field>
	</row>
	<row>
		<field name="help_topic_id">421</field>
		<field name="name">DECLARE CURSOR</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
DECLARE cursor_name CURSOR FOR select_statement

This statement declares a cursor and associates it with a SELECT
statement that retrieves the rows to be traversed by the cursor. To
fetch the rows later, use a FETCH statement. The number of columns
retrieved by the SELECT statement must match the number of output
variables specified in the FETCH statement.

The SELECT statement cannot have an INTO clause.

Cursor declarations must appear before handler declarations and after
variable and condition declarations.

A stored program may contain multiple cursor declarations, but each
cursor declared in a given block must have a unique name. For an
example, see http://dev.mysql.com/doc/refman/5.5/en/cursors.html.

For information available through SHOW statements, it is possible in
many cases to obtain equivalent information by using a cursor with an
INFORMATION_SCHEMA table.

URL: http://dev.mysql.com/doc/refman/5.5/en/declare-cursor.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/declare-cursor.html</field>
	</row>
	<row>
		<field name="help_topic_id">422</field>
		<field name="name">LOAD DATA</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
LOAD DATA [LOW_PRIORITY | CONCURRENT] [LOCAL] INFILE 'file_name'
    [REPLACE | IGNORE]
    INTO TABLE tbl_name
    [CHARACTER SET charset_name]
    [{FIELDS | COLUMNS}
        [TERMINATED BY 'string']
        [[OPTIONALLY] ENCLOSED BY 'char']
        [ESCAPED BY 'char']
    ]
    [LINES
        [STARTING BY 'string']
        [TERMINATED BY 'string']
    ]
    [IGNORE number {LINES | ROWS}]
    [(col_name_or_user_var,...)]
    [SET col_name = expr,...]

The LOAD DATA INFILE statement reads rows from a text file into a table
at a very high speed. The file name must be given as a literal string.

LOAD DATA INFILE is the complement of SELECT ... INTO OUTFILE. (See
http://dev.mysql.com/doc/refman/5.5/en/select-into.html.) To write data
from a table to a file, use SELECT ... INTO OUTFILE. To read the file
back into a table, use LOAD DATA INFILE. The syntax of the FIELDS and
LINES clauses is the same for both statements. Both clauses are
optional, but FIELDS must precede LINES if both are specified.

For more information about the efficiency of INSERT versus LOAD DATA
INFILE and speeding up LOAD DATA INFILE, see
http://dev.mysql.com/doc/refman/5.5/en/insert-speed.html.

The character set indicated by the character_set_database system
variable is used to interpret the information in the file. SET NAMES
and the setting of character_set_client do not affect interpretation of
input. If the contents of the input file use a character set that
differs from the default, it is usually preferable to specify the
character set of the file by using the CHARACTER SET clause. A
character set of binary specifies &quot;no conversion.&quot;

LOAD DATA INFILE interprets all fields in the file as having the same
character set, regardless of the data types of the columns into which
field values are loaded. For proper interpretation of file contents,
you must ensure that it was written with the correct character set. For
example, if you write a data file with mysqldump -T or by issuing a
SELECT ... INTO OUTFILE statement in mysql, be sure to use a
--default-character-set option with mysqldump or mysql so that output
is written in the character set to be used when the file is loaded with
LOAD DATA INFILE.

*Note*: It is not possible to load data files that use the ucs2, utf16,
or utf32 character set.

The character_set_filesystem system variable controls the
interpretation of the file name.

You can also load data files by using the mysqlimport utility; it
operates by sending a LOAD DATA INFILE statement to the server. The
--local option causes mysqlimport to read data files from the client
host. You can specify the --compress option to get better performance
over slow networks if the client and server support the compressed
protocol. See http://dev.mysql.com/doc/refman/5.5/en/mysqlimport.html.

If you use LOW_PRIORITY, execution of the LOAD DATA statement is
delayed until no other clients are reading from the table. This affects
only storage engines that use only table-level locking (such as MyISAM,
MEMORY, and MERGE).

If you specify CONCURRENT with a MyISAM table that satisfies the
condition for concurrent inserts (that is, it contains no free blocks
in the middle), other threads can retrieve data from the table while
LOAD DATA is executing. Using this option affects the performance of
LOAD DATA a bit, even if no other thread is using the table at the same
time.

Prior to MySQL 5.5.1, CONCURRENT was not replicated when using
statement-based replication (see Bug #34628). However, it is replicated
when using row-based replication, regardless of the version. See
http://dev.mysql.com/doc/refman/5.5/en/replication-features-load-data.h
tml, for more information.

The LOCAL keyword, if specified, is interpreted with respect to the
client end of the connection:

o If LOCAL is specified, the file is read by the client program on the
  client host and sent to the server. The file can be given as a full
  path name to specify its exact location. If given as a relative path
  name, the name is interpreted relative to the directory in which the
  client program was started.

  When using LOCAL with LOAD DATA, a copy of the file is created in the
  server's temporary directory. This is not the directory determined by
  the value of tmpdir or slave_load_tmpdir, but rather the operating
  system's temporary directory, and is not configurable in the MySQL
  Server. (Typically the system temporary directory is /tmp on Linux
  systems and C:\WINDOWS\TEMP on Windows.) Lack of sufficient space for
  the copy in this directory can cause the LOAD DATA LOCAL statement to
  fail.

o If LOCAL is not specified, the file must be located on the server
  host and is read directly by the server. The server uses the
  following rules to locate the file:

  o If the file name is an absolute path name, the server uses it as
    given.

  o If the file name is a relative path name with one or more leading
    components, the server searches for the file relative to the
    server's data directory.

  o If a file name with no leading components is given, the server
    looks for the file in the database directory of the default
    database.

Note that, in the non-LOCAL case, these rules mean that a file named as
./myfile.txt is read from the server's data directory, whereas the file
named as myfile.txt is read from the database directory of the default
database. For example, if db1 is the default database, the following
LOAD DATA statement reads the file data.txt from the database directory
for db1, even though the statement explicitly loads the file into a
table in the db2 database:

LOAD DATA INFILE 'data.txt' INTO TABLE db2.my_table;

Windows path names are specified using forward slashes rather than
backslashes. If you do use backslashes, you must double them.

For security reasons, when reading text files located on the server,
the files must either reside in the database directory or be readable
by all. Also, to use LOAD DATA INFILE on server files, you must have
the FILE privilege. See
http://dev.mysql.com/doc/refman/5.5/en/privileges-provided.html. For
non-LOCAL load operations, if the secure_file_priv system variable is
set to a nonempty directory name, the file to be loaded must be located
in that directory.

URL: http://dev.mysql.com/doc/refman/5.5/en/load-data.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/load-data.html</field>
	</row>
	<row>
		<field name="help_topic_id">423</field>
		<field name="name">MULTILINESTRING</field>
		<field name="help_category_id">25</field>
		<field name="description">MultiLineString(ls1,ls2,...)

Constructs a MultiLineString value using LineString or WKB LineString
arguments.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">424</field>
		<field name="name">LOCALTIME</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
LOCALTIME, LOCALTIME()

LOCALTIME and LOCALTIME() are synonyms for NOW().

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">425</field>
		<field name="name">SHOW RELAYLOG EVENTS</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW RELAYLOG EVENTS
   [IN 'log_name'] [FROM pos] [LIMIT [offset,] row_count]

Shows the events in the relay log of a replication slave. If you do not
specify 'log_name', the first relay log is displayed. This statement
has no effect on the master.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-relaylog-events.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-relaylog-events.html</field>
	</row>
	<row>
		<field name="help_topic_id">426</field>
		<field name="name">MPOINTFROMTEXT</field>
		<field name="help_category_id">3</field>
		<field name="description">MPointFromText(wkt[,srid]), MultiPointFromText(wkt[,srid])

Constructs a MULTIPOINT value using its WKT representation and SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">427</field>
		<field name="name">BLOB</field>
		<field name="help_category_id">23</field>
		<field name="description">BLOB[(M)]

A BLOB column with a maximum length of 65,535 (216 - 1) bytes. Each
BLOB value is stored using a 2-byte length prefix that indicates the
number of bytes in the value.

An optional length M can be given for this type. If this is done, MySQL
creates the column as the smallest BLOB type large enough to hold
values M bytes long.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">428</field>
		<field name="name">SHA1</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
SHA1(str), SHA(str)

Calculates an SHA-1 160-bit checksum for the string, as described in
RFC 3174 (Secure Hash Algorithm). The value is returned as a string of
40 hex digits, or NULL if the argument was NULL. One of the possible
uses for this function is as a hash key. See the notes at the beginning
of this section about storing hash values efficiently. You can also use
SHA1() as a cryptographic function for storing passwords. SHA() is
synonymous with SHA1().

As of MySQL 5.5.3, the return value is a nonbinary string in the
connection character set. Before 5.5.3, the return value is a binary
string; see the notes at the beginning of this section about using the
value as a nonbinary string.

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example">mysql&gt; SELECT SHA1('abc');
        -&gt; 'a9993e364706816aba3e25717850c26c9cd0d89d'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">429</field>
		<field name="name">SUBSTR</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
SUBSTR(str,pos), SUBSTR(str FROM pos), SUBSTR(str,pos,len), SUBSTR(str
FROM pos FOR len)

SUBSTR() is a synonym for SUBSTRING().

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">430</field>
		<field name="name">PASSWORD</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
PASSWORD(str)

Calculates and returns a hashed password string from the plaintext
password str and returns a nonbinary string in the connection character
set (a binary string before MySQL 5.5.3), or NULL if the argument is
NULL. This function is the SQL interface to the algorithm used by the
server to encrypt MySQL passwords for storage in the mysql.user grant
table.

The password hashing method used by PASSWORD() depends on the value of
the old_passwords system variable:

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example">mysql&gt; SET old_passwords = 0;
mysql&gt; SELECT PASSWORD('mypass');
+-------------------------------------------+
| PASSWORD('mypass')                        |
+-------------------------------------------+
| *6C8989366EAF75BB670AD8EA7A7FC1176A95CEF4 |
+-------------------------------------------+

mysql&gt; SET old_passwords = 1;
mysql&gt; SELECT PASSWORD('mypass');
+--------------------+
| PASSWORD('mypass') |
+--------------------+
| 6f8c114b58f2ce9e   |
+--------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">431</field>
		<field name="name">CHAR</field>
		<field name="help_category_id">23</field>
		<field name="description">[NATIONAL] CHAR[(M)] [CHARACTER SET charset_name] [COLLATE
collation_name]

A fixed-length string that is always right-padded with spaces to the
specified length when stored. M represents the column length in
characters. The range of M is 0 to 255. If M is omitted, the length is
1.

*Note*: Trailing spaces are removed when CHAR values are retrieved
unless the PAD_CHAR_TO_FULL_LENGTH SQL mode is enabled.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">432</field>
		<field name="name">UTC_DATE</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
UTC_DATE, UTC_DATE()

Returns the current UTC date as a value in 'YYYY-MM-DD' or YYYYMMDD
format, depending on whether the function is used in a string or
numeric context.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT UTC_DATE(), UTC_DATE() + 0;
        -&gt; '2003-08-14', 20030814
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">433</field>
		<field name="name">DIMENSION</field>
		<field name="help_category_id">37</field>
		<field name="description">Dimension(g)

Returns the inherent dimension of the geometry value g. The result can
be -1, 0, 1, or 2. The meaning of these values is given in
http://dev.mysql.com/doc/refman/5.5/en/gis-class-geometry.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SELECT Dimension(GeomFromText('LineString(1 1,2 2)'));
+------------------------------------------------+
| Dimension(GeomFromText('LineString(1 1,2 2)')) |
+------------------------------------------------+
|                                              1 |
+------------------------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">434</field>
		<field name="name">COUNT DISTINCT</field>
		<field name="help_category_id">16</field>
		<field name="description">Syntax:
COUNT(DISTINCT expr,[expr...])

Returns a count of the number of rows with different non-NULL expr
values.

COUNT(DISTINCT) returns 0 if there were no matching rows.

URL: http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html

</field>
		<field name="example">mysql&gt; SELECT COUNT(DISTINCT results) FROM student;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/group-by-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">435</field>
		<field name="name">BIT</field>
		<field name="help_category_id">23</field>
		<field name="description">BIT[(M)]

A bit-field type. M indicates the number of bits per value, from 1 to
64. The default is 1 if M is omitted.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">436</field>
		<field name="name">XA</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
XA {START|BEGIN} xid [JOIN|RESUME]

XA END xid [SUSPEND [FOR MIGRATE]]

XA PREPARE xid

XA COMMIT xid [ONE PHASE]

XA ROLLBACK xid

XA RECOVER

For XA START, the JOIN and RESUME clauses are not supported.

For XA END the SUSPEND [FOR MIGRATE] clause is not supported.

Each XA statement begins with the XA keyword, and most of them require
an xid value. An xid is an XA transaction identifier. It indicates
which transaction the statement applies to. xid values are supplied by
the client, or generated by the MySQL server. An xid value has from one
to three parts:

xid: gtrid [, bqual [, formatID ]]

gtrid is a global transaction identifier, bqual is a branch qualifier,
and formatID is a number that identifies the format used by the gtrid
and bqual values. As indicated by the syntax, bqual and formatID are
optional. The default bqual value is '' if not given. The default
formatID value is 1 if not given.

gtrid and bqual must be string literals, each up to 64 bytes (not
characters) long. gtrid and bqual can be specified in several ways. You
can use a quoted string ('ab'), hex string (0x6162, X'ab'), or bit
value (b'nnnn').

formatID is an unsigned integer.

The gtrid and bqual values are interpreted in bytes by the MySQL
server's underlying XA support routines. However, while an SQL
statement containing an XA statement is being parsed, the server works
with some specific character set. To be safe, write gtrid and bqual as
hex strings.

xid values typically are generated by the Transaction Manager. Values
generated by one TM must be different from values generated by other
TMs. A given TM must be able to recognize its own xid values in a list
of values returned by the XA RECOVER statement.

XA START xid starts an XA transaction with the given xid value. Each XA
transaction must have a unique xid value, so the value must not
currently be used by another XA transaction. Uniqueness is assessed
using the gtrid and bqual values. All following XA statements for the
XA transaction must be specified using the same xid value as that given
in the XA START statement. If you use any of those statements but
specify an xid value that does not correspond to some existing XA
transaction, an error occurs.

One or more XA transactions can be part of the same global transaction.
All XA transactions within a given global transaction must use the same
gtrid value in the xid value. For this reason, gtrid values must be
globally unique so that there is no ambiguity about which global
transaction a given XA transaction is part of. The bqual part of the
xid value must be different for each XA transaction within a global
transaction. (The requirement that bqual values be different is a
limitation of the current MySQL XA implementation. It is not part of
the XA specification.)

The XA RECOVER statement returns information for those XA transactions
on the MySQL server that are in the PREPARED state. (See
http://dev.mysql.com/doc/refman/5.5/en/xa-states.html.) The output
includes a row for each such XA transaction on the server, regardless
of which client started it.

XA RECOVER output rows look like this (for an example xid value
consisting of the parts 'abc', 'def', and 7):

mysql&gt; XA RECOVER;
+----------+--------------+--------------+--------+
| formatID | gtrid_length | bqual_length | data   |
+----------+--------------+--------------+--------+
|        7 |            3 |            3 | abcdef |
+----------+--------------+--------------+--------+

The output columns have the following meanings:

o formatID is the formatID part of the transaction xid

o gtrid_length is the length in bytes of the gtrid part of the xid

o bqual_length is the length in bytes of the bqual part of the xid

o data is the concatenation of the gtrid and bqual parts of the xid

URL: http://dev.mysql.com/doc/refman/5.5/en/xa-statements.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/xa-statements.html</field>
	</row>
	<row>
		<field name="help_topic_id">437</field>
		<field name="name">EQUALS</field>
		<field name="help_category_id">31</field>
		<field name="description">Equals(g1,g2)

Returns 1 or 0 to indicate whether g1 is spatially equal to g2.

URL: http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/functions-for-testing-spatial-relations-between-geometric-objects.html</field>
	</row>
	<row>
		<field name="help_topic_id">438</field>
		<field name="name">SHOW CREATE VIEW</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW CREATE VIEW view_name

This statement shows a CREATE VIEW statement that creates the given
view.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-create-view.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-create-view.html</field>
	</row>
	<row>
		<field name="help_topic_id">439</field>
		<field name="name">INTERVAL</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
INTERVAL(N,N1,N2,N3,...)

Returns 0 if N &lt; N1, 1 if N &lt; N2 and so on or -1 if N is NULL. All
arguments are treated as integers. It is required that N1 &lt; N2 &lt; N3 &lt;
... &lt; Nn for this function to work correctly. This is because a binary
search is used (very fast).

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT INTERVAL(23, 1, 15, 17, 30, 44, 200);
        -&gt; 3
mysql&gt; SELECT INTERVAL(10, 1, 10, 100, 1000);
        -&gt; 2
mysql&gt; SELECT INTERVAL(22, 23, 30, 44, 200);
        -&gt; 0
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">440</field>
		<field name="name">FROM_DAYS</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
FROM_DAYS(N)

Given a day number N, returns a DATE value.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT FROM_DAYS(730669);
        -&gt; '2007-07-03'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">441</field>
		<field name="name">ALTER PROCEDURE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
ALTER PROCEDURE proc_name [characteristic ...]

characteristic:
    COMMENT 'string'
  | LANGUAGE SQL
  | { CONTAINS SQL | NO SQL | READS SQL DATA | MODIFIES SQL DATA }
  | SQL SECURITY { DEFINER | INVOKER }

This statement can be used to change the characteristics of a stored
procedure. More than one change may be specified in an ALTER PROCEDURE
statement. However, you cannot change the parameters or body of a
stored procedure using this statement; to make such changes, you must
drop and re-create the procedure using DROP PROCEDURE and CREATE
PROCEDURE.

You must have the ALTER ROUTINE privilege for the procedure. By
default, that privilege is granted automatically to the procedure
creator. This behavior can be changed by disabling the
automatic_sp_privileges system variable. See
http://dev.mysql.com/doc/refman/5.5/en/stored-routines-privileges.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/alter-procedure.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/alter-procedure.html</field>
	</row>
	<row>
		<field name="help_topic_id">442</field>
		<field name="name">BIT_COUNT</field>
		<field name="help_category_id">20</field>
		<field name="description">Syntax:
BIT_COUNT(N)

Returns the number of bits that are set in the argument N.

URL: http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html

</field>
		<field name="example">mysql&gt; SELECT BIT_COUNT(29), BIT_COUNT(b'101010');
        -&gt; 4, 3
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/bit-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">443</field>
		<field name="name">OCTET_LENGTH</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
OCTET_LENGTH(str)

OCTET_LENGTH() is a synonym for LENGTH().

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">444</field>
		<field name="name">UTC_TIMESTAMP</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
UTC_TIMESTAMP, UTC_TIMESTAMP()

Returns the current UTC date and time as a value in 'YYYY-MM-DD
HH:MM:SS' or YYYYMMDDHHMMSS.uuuuuu format, depending on whether the
function is used in a string or numeric context.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT UTC_TIMESTAMP(), UTC_TIMESTAMP() + 0;
        -&gt; '2003-08-14 18:08:04', 20030814180804.000000
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">445</field>
		<field name="name">AES_ENCRYPT</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
AES_ENCRYPT(str,key_str)

AES_ENCRYPT() and AES_DECRYPT() enable encryption and decryption of
data using the official AES (Advanced Encryption Standard) algorithm,
previously known as &quot;Rijndael.&quot; Encoding with a 128-bit key length is
used, but you can extend it up to 256 bits by modifying the source. We
chose 128 bits because it is much faster and it is secure enough for
most purposes.

AES_ENCRYPT() encrypts a string and returns a binary string.
AES_DECRYPT() decrypts the encrypted string and returns the original
string. The input arguments may be any length. If either argument is
NULL, the result of this function is also NULL.

Because AES is a block-level algorithm, padding is used to encode
uneven length strings and so the result string length may be calculated
using this formula:

16 * (trunc(string_length / 16) + 1)

If AES_DECRYPT() detects invalid data or incorrect padding, it returns
NULL. However, it is possible for AES_DECRYPT() to return a non-NULL
value (possibly garbage) if the input data or the key is invalid.

You can use the AES functions to store data in an encrypted form by
modifying your queries:

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example">INSERT INTO t VALUES (1,AES_ENCRYPT('text','password'));
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">446</field>
		<field name="name">+</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
+

Addition:

URL: http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html

</field>
		<field name="example">mysql&gt; SELECT 3+5;
        -&gt; 8
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/arithmetic-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">447</field>
		<field name="name">INET_NTOA</field>
		<field name="help_category_id">14</field>
		<field name="description">Syntax:
INET_NTOA(expr)

Given a numeric IPv4 network address in network byte order, returns the
dotted-quad representation of the address as a string. INET_NTOA()
returns NULL if it does not understand its argument.

As of MySQL 5.5.3, the return value is a nonbinary string in the
connection character set. Before 5.5.3, the return value is a binary
string.

URL: http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html

</field>
		<field name="example">mysql&gt; SELECT INET_NTOA(167773449);
        -&gt; '10.0.5.9'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/miscellaneous-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">448</field>
		<field name="name">ACOS</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
ACOS(X)

Returns the arc cosine of X, that is, the value whose cosine is X.
Returns NULL if X is not in the range -1 to 1.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT ACOS(1);
        -&gt; 0
mysql&gt; SELECT ACOS(1.0001);
        -&gt; NULL
mysql&gt; SELECT ACOS(0);
        -&gt; 1.5707963267949
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">449</field>
		<field name="name">ISOLATION</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
SET [GLOBAL | SESSION] TRANSACTION ISOLATION LEVEL
  {
       REPEATABLE READ
     | READ COMMITTED
     | READ UNCOMMITTED
     | SERIALIZABLE
   }

This statement sets the transaction isolation level, used for
operations on InnoDB tables.

Scope of the Isolation Level

You can set the isolation level globally, for the current session, or
for the next transaction:

o With the GLOBAL keyword, the statement sets the default transaction
  level globally for all subsequent sessions. Existing sessions are
  unaffected.

o With the SESSION keyword, the statement sets the default transaction
  level for all subsequent transactions performed within the current
  session.

o Without any SESSION or GLOBAL keyword, the statement sets the
  isolation level for the next (not started) transaction performed
  within the current session.

A change to the global default isolation level requires the SUPER
privilege. Any session is free to change its session isolation level
(even in the middle of a transaction), or the isolation level for its
next transaction.

SET TRANSACTION ISOLATION LEVEL without GLOBAL or SESSION is not
permitted while there is an active transaction:

mysql&gt; START TRANSACTION;
Query OK, 0 rows affected (0.02 sec)

mysql&gt; SET TRANSACTION ISOLATION LEVEL SERIALIZABLE;
ERROR 1568 (25001): Transaction isolation level can't be changed
while a transaction is in progress

To set the global default isolation level at server startup, use the
--transaction-isolation=level option to mysqld on the command line or
in an option file. Values of level for this option use dashes rather
than spaces, so the permissible values are READ-UNCOMMITTED,
READ-COMMITTED, REPEATABLE-READ, or SERIALIZABLE. For example, to set
the default isolation level to REPEATABLE READ, use these lines in the
[mysqld] section of an option file:

[mysqld]
transaction-isolation = REPEATABLE-READ

It is possible to check or set the global and session transaction
isolation levels at runtime by using the tx_isolation system variable:

SELECT @@GLOBAL.tx_isolation, @@tx_isolation;
SET GLOBAL tx_isolation='REPEATABLE-READ';
SET SESSION tx_isolation='SERIALIZABLE';

Details and Usage of Isolation Levels

InnoDB supports each of the transaction isolation levels described here
using different locking strategies. You can enforce a high degree of
consistency with the default REPEATABLE READ level, for operations on
crucial data where ACID compliance is important. Or you can relax the
consistency rules with READ COMMITTED or even READ UNCOMMITTED, in
situations such as bulk reporting where precise consistency and
repeatable results are less important than minimizing the amount of
overhead for locking. SERIALIZABLE enforces even stricter rules than
REPEATABLE READ, and is used mainly in specialized situations, such as
with XA transactions and for troubleshooting issues with concurrency
and deadlocks.

For full information about how these isolation levels work with InnoDB
transactions, see
http://dev.mysql.com/doc/refman/5.5/en/innodb-transaction-model.html.
In particular, for additional information about InnoDB record-level
locks and how it uses them to execute various types of statements, see
http://dev.mysql.com/doc/refman/5.5/en/innodb-record-level-locks.html
and http://dev.mysql.com/doc/refman/5.5/en/innodb-locks-set.html.

The following list describes how MySQL supports the different
transaction levels. The list goes from the most commonly used level to
the least used.

o REPEATABLE READ

  This is the default isolation level for InnoDB. For consistent reads,
  there is an important difference from the READ COMMITTED isolation
  level: All consistent reads within the same transaction read the
  snapshot established by the first read. This convention means that if
  you issue several plain (nonlocking) SELECT statements within the
  same transaction, these SELECT statements are consistent also with
  respect to each other. See
  http://dev.mysql.com/doc/refman/5.5/en/innodb-consistent-read.html.

  For locking reads (SELECT with FOR UPDATE or LOCK IN SHARE MODE),
  UPDATE, and DELETE statements, locking depends on whether the
  statement uses a unique index with a unique search condition, or a
  range-type search condition. For a unique index with a unique search
  condition, InnoDB locks only the index record found, not the gap
  before it. For other search conditions, InnoDB locks the index range
  scanned, using gap locks or next-key (gap plus index-record) locks to
  block insertions by other sessions into the gaps covered by the
  range.

o READ COMMITTED

  A somewhat Oracle-like isolation level with respect to consistent
  (nonlocking) reads: Each consistent read, even within the same
  transaction, sets and reads its own fresh snapshot. See
  http://dev.mysql.com/doc/refman/5.5/en/innodb-consistent-read.html.

  For locking reads (SELECT with FOR UPDATE or LOCK IN SHARE MODE),
  InnoDB locks only index records, not the gaps before them, and thus
  permits the free insertion of new records next to locked records. For
  UPDATE and DELETE statements, locking depends on whether the
  statement uses a unique index with a unique search condition (such as
  WHERE id = 100), or a range-type search condition (such as WHERE id &gt;
  100). For a unique index with a unique search condition, InnoDB locks
  only the index record found, not the gap before it. For range-type
  searches, InnoDB locks the index range scanned, using gap locks or
  next-key (gap plus index-record) locks to block insertions by other
  sessions into the gaps covered by the range. This is necessary
  because &quot;phantom rows&quot; must be blocked for MySQL replication and
  recovery to work.

  *Note*: In MySQL 5.5, if the READ COMMITTED isolation level is used
  or the innodb_locks_unsafe_for_binlog system variable is enabled,
  there is no InnoDB gap locking except for foreign-key constraint
  checking and duplicate-key checking. Also, record locks for
  nonmatching rows are released after MySQL has evaluated the WHERE
  condition. If you use READ COMMITTED or enable
  innodb_locks_unsafe_for_binlog, you must use row-based binary
  logging.

o READ UNCOMMITTED

  SELECT statements are performed in a nonlocking fashion, but a
  possible earlier version of a row might be used. Thus, using this
  isolation level, such reads are not consistent. This is also called a
  &quot;dirty read.&quot; Otherwise, this isolation level works like READ
  COMMITTED.

o SERIALIZABLE

  This level is like REPEATABLE READ, but InnoDB implicitly converts
  all plain SELECT statements to SELECT ... LOCK IN SHARE MODE if
  autocommit is disabled. If autocommit is enabled, the SELECT is its
  own transaction. It therefore is known to be read only and can be
  serialized if performed as a consistent (nonlocking) read and need
  not block for other transactions. (To force a plain SELECT to block
  if other transactions have modified the selected rows, disable
  autocommit.)

URL: http://dev.mysql.com/doc/refman/5.5/en/set-transaction.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/set-transaction.html</field>
	</row>
	<row>
		<field name="help_topic_id">450</field>
		<field name="name">CEILING</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
CEILING(X)

Returns the smallest integer value not less than X.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT CEILING(1.23);
        -&gt; 2
mysql&gt; SELECT CEILING(-1.23);
        -&gt; -1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">451</field>
		<field name="name">SIN</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
SIN(X)

Returns the sine of X, where X is given in radians.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT SIN(PI());
        -&gt; 1.2246063538224e-16
mysql&gt; SELECT ROUND(SIN(PI()));
        -&gt; 0
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">452</field>
		<field name="name">DAYOFWEEK</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
DAYOFWEEK(date)

Returns the weekday index for date (1 = Sunday, 2 = Monday, ..., 7 =
Saturday). These index values correspond to the ODBC standard.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT DAYOFWEEK('2007-02-03');
        -&gt; 7
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">453</field>
		<field name="name">SHOW PROCESSLIST</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW [FULL] PROCESSLIST

SHOW PROCESSLIST shows you which threads are running. You can also get
this information from the INFORMATION_SCHEMA PROCESSLIST table or the
mysqladmin processlist command. If you have the PROCESS privilege, you
can see all threads. Otherwise, you can see only your own threads (that
is, threads associated with the MySQL account that you are using). If
you do not use the FULL keyword, only the first 100 characters of each
statement are shown in the Info field.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-processlist.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-processlist.html</field>
	</row>
	<row>
		<field name="help_topic_id">454</field>
		<field name="name">LINEFROMWKB</field>
		<field name="help_category_id">33</field>
		<field name="description">LineFromWKB(wkb[,srid]), LineStringFromWKB(wkb[,srid])

Constructs a LINESTRING value using its WKB representation and SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">455</field>
		<field name="name">GEOMETRYTYPE</field>
		<field name="help_category_id">37</field>
		<field name="description">GeometryType(g)

Returns as a binary string the name of the geometry type of which the
geometry instance g is a member. The name corresponds to one of the
instantiable Geometry subclasses.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SELECT GeometryType(GeomFromText('POINT(1 1)'));
+------------------------------------------+
| GeometryType(GeomFromText('POINT(1 1)')) |
+------------------------------------------+
| POINT                                    |
+------------------------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">456</field>
		<field name="name">CREATE VIEW</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
CREATE
    [OR REPLACE]
    [ALGORITHM = {UNDEFINED | MERGE | TEMPTABLE}]
    [DEFINER = { user | CURRENT_USER }]
    [SQL SECURITY { DEFINER | INVOKER }]
    VIEW view_name [(column_list)]
    AS select_statement
    [WITH [CASCADED | LOCAL] CHECK OPTION]

The CREATE VIEW statement creates a new view, or replaces an existing
one if the OR REPLACE clause is given. If the view does not exist,
CREATE OR REPLACE VIEW is the same as CREATE VIEW. If the view does
exist, CREATE OR REPLACE VIEW is the same as ALTER VIEW.

The select_statement is a SELECT statement that provides the definition
of the view. (When you select from the view, you select in effect using
the SELECT statement.) select_statement can select from base tables or
other views.

The view definition is &quot;frozen&quot; at creation time, so changes to the
underlying tables afterward do not affect the view definition. For
example, if a view is defined as SELECT * on a table, new columns added
to the table later do not become part of the view.

The ALGORITHM clause affects how MySQL processes the view. The DEFINER
and SQL SECURITY clauses specify the security context to be used when
checking access privileges at view invocation time. The WITH CHECK
OPTION clause can be given to constrain inserts or updates to rows in
tables referenced by the view. These clauses are described later in
this section.

The CREATE VIEW statement requires the CREATE VIEW privilege for the
view, and some privilege for each column selected by the SELECT
statement. For columns used elsewhere in the SELECT statement you must
have the SELECT privilege. If the OR REPLACE clause is present, you
must also have the DROP privilege for the view. CREATE VIEW might also
require the SUPER privilege, depending on the DEFINER value, as
described later in this section.

When a view is referenced, privilege checking occurs as described later
in this section.

A view belongs to a database. By default, a new view is created in the
default database. To create the view explicitly in a given database,
specify the name as db_name.view_name when you create it:

mysql&gt; CREATE VIEW test.v AS SELECT * FROM t;

Within a database, base tables and views share the same namespace, so a
base table and a view cannot have the same name.

Columns retrieved by the SELECT statement can be simple references to
table columns. They can also be expressions that use functions,
constant values, operators, and so forth.

Views must have unique column names with no duplicates, just like base
tables. By default, the names of the columns retrieved by the SELECT
statement are used for the view column names. To define explicit names
for the view columns, the optional column_list clause can be given as a
list of comma-separated identifiers. The number of names in column_list
must be the same as the number of columns retrieved by the SELECT
statement.

Unqualified table or view names in the SELECT statement are interpreted
with respect to the default database. A view can refer to tables or
views in other databases by qualifying the table or view name with the
proper database name.

A view can be created from many kinds of SELECT statements. It can
refer to base tables or other views. It can use joins, UNION, and
subqueries. The SELECT need not even refer to any tables. The following
example defines a view that selects two columns from another table, as
well as an expression calculated from those columns:

mysql&gt; CREATE TABLE t (qty INT, price INT);
mysql&gt; INSERT INTO t VALUES(3, 50);
mysql&gt; CREATE VIEW v AS SELECT qty, price, qty*price AS value FROM t;
mysql&gt; SELECT * FROM v;
+------+-------+-------+
| qty  | price | value |
+------+-------+-------+
|    3 |    50 |   150 |
+------+-------+-------+

A view definition is subject to the following restrictions:

o The SELECT statement cannot contain a subquery in the FROM clause.

o The SELECT statement cannot refer to system or user variables.

o Within a stored program, the definition cannot refer to program
  parameters or local variables.

o The SELECT statement cannot refer to prepared statement parameters.

o Any table or view referred to in the definition must exist. However,
  after a view has been created, it is possible to drop a table or view
  that the definition refers to. In this case, use of the view results
  in an error. To check a view definition for problems of this kind,
  use the CHECK TABLE statement.

o The definition cannot refer to a TEMPORARY table, and you cannot
  create a TEMPORARY view.

o Any tables named in the view definition must exist at definition
  time.

o You cannot associate a trigger with a view.

o Aliases for column names in the SELECT statement are checked against
  the maximum column length of 64 characters (not the maximum alias
  length of 256 characters).

ORDER BY is permitted in a view definition, but it is ignored if you
select from a view using a statement that has its own ORDER BY.

For other options or clauses in the definition, they are added to the
options or clauses of the statement that references the view, but the
effect is undefined. For example, if a view definition includes a LIMIT
clause, and you select from the view using a statement that has its own
LIMIT clause, it is undefined which limit applies. This same principle
applies to options such as ALL, DISTINCT, or SQL_SMALL_RESULT that
follow the SELECT keyword, and to clauses such as INTO, FOR UPDATE,
LOCK IN SHARE MODE, and PROCEDURE.

If you create a view and then change the query processing environment
by changing system variables, that may affect the results that you get
from the view:

mysql&gt; CREATE VIEW v (mycol) AS SELECT 'abc';
Query OK, 0 rows affected (0.01 sec)

mysql&gt; SET sql_mode = '';
Query OK, 0 rows affected (0.00 sec)

mysql&gt; SELECT &quot;mycol&quot; FROM v;
+-------+
| mycol |
+-------+
| mycol |
+-------+
1 row in set (0.01 sec)

mysql&gt; SET sql_mode = 'ANSI_QUOTES';
Query OK, 0 rows affected (0.00 sec)

mysql&gt; SELECT &quot;mycol&quot; FROM v;
+-------+
| mycol |
+-------+
| abc   |
+-------+
1 row in set (0.00 sec)

The DEFINER and SQL SECURITY clauses determine which MySQL account to
use when checking access privileges for the view when a statement is
executed that references the view. The valid SQL SECURITY
characteristic values are DEFINER and INVOKER. These indicate that the
required privileges must be held by the user who defined or invoked the
view, respectively. The default SQL SECURITY value is DEFINER.

If a user value is given for the DEFINER clause, it should be a MySQL
account specified as 'user_name'@'host_name' (the same format used in
the GRANT statement), CURRENT_USER, or CURRENT_USER(). The default
DEFINER value is the user who executes the CREATE VIEW statement. This
is the same as specifying DEFINER = CURRENT_USER explicitly.

If you specify the DEFINER clause, these rules determine the valid
DEFINER user values:

o If you do not have the SUPER privilege, the only valid user value is
  your own account, either specified literally or by using
  CURRENT_USER. You cannot set the definer to some other account.

o If you have the SUPER privilege, you can specify any syntactically
  valid account name. If the account does not actually exist, a warning
  is generated.

o Although it is possible to create a view with a nonexistent DEFINER
  account, an error occurs when the view is referenced if the SQL
  SECURITY value is DEFINER but the definer account does not exist.

For more information about view security, see
http://dev.mysql.com/doc/refman/5.5/en/stored-programs-security.html.

Within a view definition, CURRENT_USER returns the view's DEFINER value
by default. For views defined with the SQL SECURITY INVOKER
characteristic, CURRENT_USER returns the account for the view's
invoker. For information about user auditing within views, see
http://dev.mysql.com/doc/refman/5.5/en/account-activity-auditing.html.

Within a stored routine that is defined with the SQL SECURITY DEFINER
characteristic, CURRENT_USER returns the routine's DEFINER value. This
also affects a view defined within such a routine, if the view
definition contains a DEFINER value of CURRENT_USER.

View privileges are checked like this:

o At view definition time, the view creator must have the privileges
  needed to use the top-level objects accessed by the view. For
  example, if the view definition refers to table columns, the creator
  must have some privilege for each column in the select list of the
  definition, and the SELECT privilege for each column used elsewhere
  in the definition. If the definition refers to a stored function,
  only the privileges needed to invoke the function can be checked. The
  privileges required at function invocation time can be checked only
  as it executes: For different invocations, different execution paths
  within the function might be taken.

o The user who references a view must have appropriate privileges to
  access it (SELECT to select from it, INSERT to insert into it, and so
  forth.)

o When a view has been referenced, privileges for objects accessed by
  the view are checked against the privileges held by the view DEFINER
  account or invoker, depending on whether the SQL SECURITY
  characteristic is DEFINER or INVOKER, respectively.

o If reference to a view causes execution of a stored function,
  privilege checking for statements executed within the function depend
  on whether the function SQL SECURITY characteristic is DEFINER or
  INVOKER. If the security characteristic is DEFINER, the function runs
  with the privileges of the DEFINER account. If the characteristic is
  INVOKER, the function runs with the privileges determined by the
  view's SQL SECURITY characteristic.

Example: A view might depend on a stored function, and that function
might invoke other stored routines. For example, the following view
invokes a stored function f():

CREATE VIEW v AS SELECT * FROM t WHERE t.id = f(t.name);

Suppose that f() contains a statement such as this:

IF name IS NULL then
  CALL p1();
ELSE
  CALL p2();
END IF;

The privileges required for executing statements within f() need to be
checked when f() executes. This might mean that privileges are needed
for p1() or p2(), depending on the execution path within f(). Those
privileges must be checked at runtime, and the user who must possess
the privileges is determined by the SQL SECURITY values of the view v
and the function f().

The DEFINER and SQL SECURITY clauses for views are extensions to
standard SQL. In standard SQL, views are handled using the rules for
SQL SECURITY DEFINER. The standard says that the definer of the view,
which is the same as the owner of the view's schema, gets applicable
privileges on the view (for example, SELECT) and may grant them. MySQL
has no concept of a schema &quot;owner&quot;, so MySQL adds a clause to identify
the definer. The DEFINER clause is an extension where the intent is to
have what the standard has; that is, a permanent record of who defined
the view. This is why the default DEFINER value is the account of the
view creator.

The optional ALGORITHM clause is a MySQL extension to standard SQL. It
affects how MySQL processes the view. ALGORITHM takes three values:
MERGE, TEMPTABLE, or UNDEFINED. The default algorithm is UNDEFINED if
no ALGORITHM clause is present. For more information, see
http://dev.mysql.com/doc/refman/5.5/en/view-algorithms.html.

Some views are updatable. That is, you can use them in statements such
as UPDATE, DELETE, or INSERT to update the contents of the underlying
table. For a view to be updatable, there must be a one-to-one
relationship between the rows in the view and the rows in the
underlying table. There are also certain other constructs that make a
view nonupdatable.

The WITH CHECK OPTION clause can be given for an updatable view to
prevent inserts or updates to rows except those for which the WHERE
clause in the select_statement is true.

In a WITH CHECK OPTION clause for an updatable view, the LOCAL and
CASCADED keywords determine the scope of check testing when the view is
defined in terms of another view. The LOCAL keyword restricts the CHECK
OPTION only to the view being defined. CASCADED causes the checks for
underlying views to be evaluated as well. When neither keyword is
given, the default is CASCADED.

For more information about updatable views and the WITH CHECK OPTION
clause, see
http://dev.mysql.com/doc/refman/5.5/en/view-updatability.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/create-view.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-view.html</field>
	</row>
	<row>
		<field name="help_topic_id">457</field>
		<field name="name">TRIM</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
TRIM([{BOTH | LEADING | TRAILING} [remstr] FROM] str), TRIM([remstr
FROM] str)

Returns the string str with all remstr prefixes or suffixes removed. If
none of the specifiers BOTH, LEADING, or TRAILING is given, BOTH is
assumed. remstr is optional and, if not specified, spaces are removed.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT TRIM('  bar   ');
        -&gt; 'bar'
mysql&gt; SELECT TRIM(LEADING 'x' FROM 'xxxbarxxx');
        -&gt; 'barxxx'
mysql&gt; SELECT TRIM(BOTH 'x' FROM 'xxxbarxxx');
        -&gt; 'bar'
mysql&gt; SELECT TRIM(TRAILING 'xyz' FROM 'barxxyz');
        -&gt; 'barx'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">458</field>
		<field name="name">IS</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
IS boolean_value

Tests a value against a boolean value, where boolean_value can be TRUE,
FALSE, or UNKNOWN.

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT 1 IS TRUE, 0 IS FALSE, NULL IS UNKNOWN;
        -&gt; 1, 1, 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">459</field>
		<field name="name">GET_FORMAT</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
GET_FORMAT({DATE|TIME|DATETIME}, {'EUR'|'USA'|'JIS'|'ISO'|'INTERNAL'})

Returns a format string. This function is useful in combination with
the DATE_FORMAT() and the STR_TO_DATE() functions.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT DATE_FORMAT('2003-10-03',GET_FORMAT(DATE,'EUR'));
        -&gt; '03.10.2003'
mysql&gt; SELECT STR_TO_DATE('10.31.2003',GET_FORMAT(DATE,'USA'));
        -&gt; '2003-10-31'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">460</field>
		<field name="name">TINYBLOB</field>
		<field name="help_category_id">23</field>
		<field name="description">TINYBLOB

A BLOB column with a maximum length of 255 (28 - 1) bytes. Each
TINYBLOB value is stored using a 1-byte length prefix that indicates
the number of bytes in the value.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">461</field>
		<field name="name">SIGNAL</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
SIGNAL condition_value
    [SET signal_information_item
    [, signal_information_item] ...]

condition_value:
    SQLSTATE [VALUE] sqlstate_value
  | condition_name

signal_information_item:
    condition_information_item_name = simple_value_specification

condition_information_item_name:
    CLASS_ORIGIN
  | SUBCLASS_ORIGIN
  | MESSAGE_TEXT
  | MYSQL_ERRNO
  | CONSTRAINT_CATALOG
  | CONSTRAINT_SCHEMA
  | CONSTRAINT_NAME
  | CATALOG_NAME
  | SCHEMA_NAME
  | TABLE_NAME
  | COLUMN_NAME
  | CURSOR_NAME

condition_name, simple_value_specification:
    (see following discussion)

SIGNAL is the way to &quot;return&quot; an error. SIGNAL provides error
information to a handler, to an outer portion of the application, or to
the client. Also, it provides control over the error's characteristics
(error number, SQLSTATE value, message). Without SIGNAL, it is
necessary to resort to workarounds such as deliberately referring to a
nonexistent table to cause a routine to return an error.

No special privileges are required to execute the SIGNAL statement.

The condition_value in a SIGNAL statement indicates the error value to
be returned. It can be an SQLSTATE value (a 5-character string literal)
or a condition_name that refers to a named condition previously defined
with DECLARE ... CONDITION (see [HELP DECLARE CONDITION]).

An SQLSTATE value can indicate errors, warnings, or &quot;not found.&quot; The
first two characters of the value indicate its error class, as
discussed in
http://dev.mysql.com/doc/refman/5.5/en/signal.html#signal-condition-inf
ormation-items. Some signal values cause statement termination; see
http://dev.mysql.com/doc/refman/5.5/en/signal.html#signal-effects.

The SQLSTATE value for a SIGNAL statement should not start with '00'
because such values indicate success and are not valid for signaling an
error. This is true whether the SQLSTATE value is specified directly in
the SIGNAL statement or in a named condition referred to in the
statement. If the value is invalid, a Bad SQLSTATE error occurs.

To signal a generic SQLSTATE value, use '45000', which means &quot;unhandled
user-defined exception.&quot;

The SIGNAL statement optionally includes a SET clause that contains
multiple signal items, in a comma-separated list of
condition_information_item_name = simple_value_specification
assignments.

Each condition_information_item_name may be specified only once in the
SET clause. Otherwise, a Duplicate condition information item error
occurs.

Valid simple_value_specification designators can be specified using
stored procedure or function parameters, stored program local variables
declared with DECLARE, user-defined variables, system variables, or
literals. A character literal may include a _charset introducer.

For information about permissible condition_information_item_name
values, see
http://dev.mysql.com/doc/refman/5.5/en/signal.html#signal-condition-inf
ormation-items.

URL: http://dev.mysql.com/doc/refman/5.5/en/signal.html

</field>
		<field name="example">CREATE PROCEDURE p (pval INT)
BEGIN
  DECLARE specialty CONDITION FOR SQLSTATE '45000';
  IF pval = 0 THEN
    SIGNAL SQLSTATE '01000';
  ELSEIF pval = 1 THEN
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'An error occurred';
  ELSEIF pval = 2 THEN
    SIGNAL specialty
      SET MESSAGE_TEXT = 'An error occurred';
  ELSE
    SIGNAL SQLSTATE '01000'
      SET MESSAGE_TEXT = 'A warning occurred', MYSQL_ERRNO = 1000;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'An error occurred', MYSQL_ERRNO = 1001;
  END IF;
END;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/signal.html</field>
	</row>
	<row>
		<field name="help_topic_id">462</field>
		<field name="name">SAVEPOINT</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
SAVEPOINT identifier
ROLLBACK [WORK] TO [SAVEPOINT] identifier
RELEASE SAVEPOINT identifier

InnoDB supports the SQL statements SAVEPOINT, ROLLBACK TO SAVEPOINT,
RELEASE SAVEPOINT and the optional WORK keyword for ROLLBACK.

URL: http://dev.mysql.com/doc/refman/5.5/en/savepoint.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/savepoint.html</field>
	</row>
	<row>
		<field name="help_topic_id">463</field>
		<field name="name">USER</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
USER()

Returns the current MySQL user name and host name as a string in the
utf8 character set.

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example">mysql&gt; SELECT USER();
        -&gt; 'davida@localhost'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">464</field>
		<field name="name">LABELS</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
[begin_label:] BEGIN
    [statement_list]
END [end_label]

[begin_label:] LOOP
    statement_list
END LOOP [end_label]

[begin_label:] REPEAT
    statement_list
UNTIL search_condition
END REPEAT [end_label]

[begin_label:] WHILE search_condition DO
    statement_list
END WHILE [end_label]

Labels are permitted for BEGIN ... END blocks and for the LOOP, REPEAT,
and WHILE statements. Label use for those statements follows these
rules:

o begin_label must be followed by a colon.

o begin_label can be given without end_label. If end_label is present,
  it must be the same as begin_label.

o end_label cannot be given without begin_label.

o Labels at the same nesting level must be distinct.

o Labels can be up to 16 characters long.

To refer to a label within the labeled construct, use an ITERATE or
LEAVE statement. The following example uses those statements to
continue iterating or terminate the loop:

CREATE PROCEDURE doiterate(p1 INT)
BEGIN
  label1: LOOP
    SET p1 = p1 + 1;
    IF p1 &lt; 10 THEN ITERATE label1; END IF;
    LEAVE label1;
  END LOOP label1;
END;

The scope of a block label does not include the code for handlers
declared within the block. For details, see [HELP DECLARE HANDLER].

URL: http://dev.mysql.com/doc/refman/5.5/en/statement-labels.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/statement-labels.html</field>
	</row>
	<row>
		<field name="help_topic_id">465</field>
		<field name="name">ALTER TABLE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
ALTER [ONLINE | OFFLINE] [IGNORE] TABLE tbl_name
    [alter_specification [, alter_specification] ...]
    [partition_options]

alter_specification:
    table_options
  | ADD [COLUMN] col_name column_definition
        [FIRST | AFTER col_name ]
  | ADD [COLUMN] (col_name column_definition,...)
  | ADD {INDEX|KEY} [index_name]
        [index_type] (index_col_name,...) [index_option] ...
  | ADD [CONSTRAINT [symbol]] PRIMARY KEY
        [index_type] (index_col_name,...) [index_option] ...
  | ADD [CONSTRAINT [symbol]]
        UNIQUE [INDEX|KEY] [index_name]
        [index_type] (index_col_name,...) [index_option] ...
  | ADD FULLTEXT [INDEX|KEY] [index_name]
        (index_col_name,...) [index_option] ...
  | ADD SPATIAL [INDEX|KEY] [index_name]
        (index_col_name,...) [index_option] ...
  | ADD [CONSTRAINT [symbol]]
        FOREIGN KEY [index_name] (index_col_name,...)
        reference_definition
  | ALTER [COLUMN] col_name {SET DEFAULT literal | DROP DEFAULT}
  | CHANGE [COLUMN] old_col_name new_col_name column_definition
        [FIRST|AFTER col_name]
  | MODIFY [COLUMN] col_name column_definition
        [FIRST | AFTER col_name]
  | DROP [COLUMN] col_name
  | DROP PRIMARY KEY
  | DROP {INDEX|KEY} index_name
  | DROP FOREIGN KEY fk_symbol
  | MAX_ROWS = rows
  | DISABLE KEYS
  | ENABLE KEYS
  | RENAME [TO|AS] new_tbl_name
  | ORDER BY col_name [, col_name] ...
  | CONVERT TO CHARACTER SET charset_name [COLLATE collation_name]
  | [DEFAULT] CHARACTER SET [=] charset_name [COLLATE [=] collation_name]
  | DISCARD TABLESPACE
  | IMPORT TABLESPACE
  | FORCE
  | ADD PARTITION (partition_definition)
  | DROP PARTITION partition_names
  | TRUNCATE PARTITION {partition_names | ALL}
  | COALESCE PARTITION number
  | REORGANIZE PARTITION [partition_names INTO (partition_definitions)]
  | ANALYZE PARTITION {partition_names | ALL}
  | CHECK PARTITION {partition_names | ALL}
  | OPTIMIZE PARTITION {partition_names | ALL}
  | REBUILD PARTITION {partition_names | ALL}
  | REPAIR PARTITION {partition_names | ALL}
  | PARTITION BY partitioning_expression
  | REMOVE PARTITIONING

index_col_name:
    col_name [(length)] [ASC | DESC]

index_type:
    USING {BTREE | HASH}

index_option:
    KEY_BLOCK_SIZE [=] value
  | index_type
  | WITH PARSER parser_name
  | COMMENT 'string'

table_options:
    table_option [[,] table_option] ...  (see CREATE TABLE options)

partition_options:
    (see CREATE TABLE options)

ALTER TABLE changes the structure of a table. For example, you can add
or delete columns, create or destroy indexes, change the type of
existing columns, or rename columns or the table itself. You can also
change characteristics such as the storage engine used for the table or
the table comment.

Partitioning-related clauses for ALTER TABLE can be used with
partitioned tables for repartitioning, for adding, dropping, merging,
and splitting partitions, and for performing partitioning maintenance.
It is possible for an ALTER TABLE statement to contain a PARTITION BY
or REMOVE PARTITION clause in an addition to other alter
specifications, but the PARTITION BY or REMOVE PARTITION clause must be
specified last after any other specifications. The ADD PARTITION, DROP
PARTITION, COALESCE PARTITION, REORGANIZE PARTITION, ANALYZE PARTITION,
CHECK PARTITION, and REPAIR PARTITION options cannot be combined with
other alter specifications in a single ALTER TABLE, since the options
just listed act on individual partitions. For more information, see
http://dev.mysql.com/doc/refman/5.5/en/alter-table-partition-operations
.html.

Following the table name, specify the alterations to be made. If none
are given, ALTER TABLE does nothing.

URL: http://dev.mysql.com/doc/refman/5.5/en/alter-table.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/alter-table.html</field>
	</row>
	<row>
		<field name="help_topic_id">466</field>
		<field name="name">MPOINTFROMWKB</field>
		<field name="help_category_id">33</field>
		<field name="description">MPointFromWKB(wkb[,srid]), MultiPointFromWKB(wkb[,srid])

Constructs a MULTIPOINT value using its WKB representation and SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">467</field>
		<field name="name">CHAR BYTE</field>
		<field name="help_category_id">23</field>
		<field name="description">The CHAR BYTE data type is an alias for the BINARY data type. This is a
compatibility feature.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">468</field>
		<field name="name">REPAIR TABLE</field>
		<field name="help_category_id">21</field>
		<field name="description">Syntax:
REPAIR [NO_WRITE_TO_BINLOG | LOCAL] TABLE
    tbl_name [, tbl_name] ...
    [QUICK] [EXTENDED] [USE_FRM]

REPAIR TABLE repairs a possibly corrupted table. By default, it has the
same effect as myisamchk --recover tbl_name. REPAIR TABLE works for
MyISAM, ARCHIVE, and CSV tables. See
http://dev.mysql.com/doc/refman/5.5/en/myisam-storage-engine.html, and
http://dev.mysql.com/doc/refman/5.5/en/archive-storage-engine.html, and
http://dev.mysql.com/doc/refman/5.5/en/csv-storage-engine.html

This statement requires SELECT and INSERT privileges for the table.

REPAIR TABLE is supported for partitioned tables. However, the USE_FRM
option cannot be used with this statement on a partitioned table.

You can use ALTER TABLE ... REPAIR PARTITION to repair one or more
partitions; for more information, see [HELP ALTER TABLE], and
http://dev.mysql.com/doc/refman/5.5/en/partitioning-maintenance.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/repair-table.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/repair-table.html</field>
	</row>
	<row>
		<field name="help_topic_id">469</field>
		<field name="name">MERGE</field>
		<field name="help_category_id">18</field>
		<field name="description">The MERGE storage engine, also known as the MRG_MyISAM engine, is a
collection of identical MyISAM tables that can be used as one.
&quot;Identical&quot; means that all tables have identical column and index
information. You cannot merge MyISAM tables in which the columns are
listed in a different order, do not have exactly the same columns, or
have the indexes in different order. However, any or all of the MyISAM
tables can be compressed with myisampack. See
http://dev.mysql.com/doc/refman/5.5/en/myisampack.html. Differences in
table options such as AVG_ROW_LENGTH, MAX_ROWS, or PACK_KEYS do not
matter.

URL: http://dev.mysql.com/doc/refman/5.5/en/merge-storage-engine.html

</field>
		<field name="example">mysql&gt; CREATE TABLE t1 (
    -&gt;    a INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    -&gt;    message CHAR(20)) ENGINE=MyISAM;
mysql&gt; CREATE TABLE t2 (
    -&gt;    a INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    -&gt;    message CHAR(20)) ENGINE=MyISAM;
mysql&gt; INSERT INTO t1 (message) VALUES ('Testing'),('table'),('t1');
mysql&gt; INSERT INTO t2 (message) VALUES ('Testing'),('table'),('t2');
mysql&gt; CREATE TABLE total (
    -&gt;    a INT NOT NULL AUTO_INCREMENT,
    -&gt;    message CHAR(20), INDEX(a))
    -&gt;    ENGINE=MERGE UNION=(t1,t2) INSERT_METHOD=LAST;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/merge-storage-engine.html</field>
	</row>
	<row>
		<field name="help_topic_id">470</field>
		<field name="name">CREATE TABLE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
CREATE [TEMPORARY] TABLE [IF NOT EXISTS] tbl_name
    (create_definition,...)
    [table_options]
    [partition_options]

Or:

CREATE [TEMPORARY] TABLE [IF NOT EXISTS] tbl_name
    [(create_definition,...)]
    [table_options]
    [partition_options]
    select_statement

Or:

CREATE [TEMPORARY] TABLE [IF NOT EXISTS] tbl_name
    { LIKE old_tbl_name | (LIKE old_tbl_name) }

create_definition:
    col_name column_definition
  | [CONSTRAINT [symbol]] PRIMARY KEY [index_type] (index_col_name,...)
      [index_option] ...
  | {INDEX|KEY} [index_name] [index_type] (index_col_name,...)
      [index_option] ...
  | [CONSTRAINT [symbol]] UNIQUE [INDEX|KEY]
      [index_name] [index_type] (index_col_name,...)
      [index_option] ...
  | {FULLTEXT|SPATIAL} [INDEX|KEY] [index_name] (index_col_name,...)
      [index_option] ...
  | [CONSTRAINT [symbol]] FOREIGN KEY
      [index_name] (index_col_name,...) reference_definition
  | CHECK (expr)

column_definition:
    data_type [NOT NULL | NULL] [DEFAULT default_value]
      [AUTO_INCREMENT] [UNIQUE [KEY] | [PRIMARY] KEY]
      [COMMENT 'string']
      [COLUMN_FORMAT {FIXED|DYNAMIC|DEFAULT}]
      [STORAGE {DISK|MEMORY|DEFAULT}]
      [reference_definition]

data_type:
    BIT[(length)]
  | TINYINT[(length)] [UNSIGNED] [ZEROFILL]
  | SMALLINT[(length)] [UNSIGNED] [ZEROFILL]
  | MEDIUMINT[(length)] [UNSIGNED] [ZEROFILL]
  | INT[(length)] [UNSIGNED] [ZEROFILL]
  | INTEGER[(length)] [UNSIGNED] [ZEROFILL]
  | BIGINT[(length)] [UNSIGNED] [ZEROFILL]
  | REAL[(length,decimals)] [UNSIGNED] [ZEROFILL]
  | DOUBLE[(length,decimals)] [UNSIGNED] [ZEROFILL]
  | FLOAT[(length,decimals)] [UNSIGNED] [ZEROFILL]
  | DECIMAL[(length[,decimals])] [UNSIGNED] [ZEROFILL]
  | NUMERIC[(length[,decimals])] [UNSIGNED] [ZEROFILL]
  | DATE
  | TIME
  | TIMESTAMP
  | DATETIME
  | YEAR
  | CHAR[(length)]
      [CHARACTER SET charset_name] [COLLATE collation_name]
  | VARCHAR(length)
      [CHARACTER SET charset_name] [COLLATE collation_name]
  | BINARY[(length)]
  | VARBINARY(length)
  | TINYBLOB
  | BLOB
  | MEDIUMBLOB
  | LONGBLOB
  | TINYTEXT [BINARY]
      [CHARACTER SET charset_name] [COLLATE collation_name]
  | TEXT [BINARY]
      [CHARACTER SET charset_name] [COLLATE collation_name]
  | MEDIUMTEXT [BINARY]
      [CHARACTER SET charset_name] [COLLATE collation_name]
  | LONGTEXT [BINARY]
      [CHARACTER SET charset_name] [COLLATE collation_name]
  | ENUM(value1,value2,value3,...)
      [CHARACTER SET charset_name] [COLLATE collation_name]
  | SET(value1,value2,value3,...)
      [CHARACTER SET charset_name] [COLLATE collation_name]
  | spatial_type

index_col_name:
    col_name [(length)] [ASC | DESC]

index_type:
    USING {BTREE | HASH}

index_option:
    KEY_BLOCK_SIZE [=] value
  | index_type
  | WITH PARSER parser_name
  | COMMENT 'string'

reference_definition:
    REFERENCES tbl_name (index_col_name,...)
      [MATCH FULL | MATCH PARTIAL | MATCH SIMPLE]
      [ON DELETE reference_option]
      [ON UPDATE reference_option]

reference_option:
    RESTRICT | CASCADE | SET NULL | NO ACTION

table_options:
    table_option [[,] table_option] ...

table_option:
    ENGINE [=] engine_name
  | AUTO_INCREMENT [=] value
  | AVG_ROW_LENGTH [=] value
  | [DEFAULT] CHARACTER SET [=] charset_name
  | CHECKSUM [=] {0 | 1}
  | [DEFAULT] COLLATE [=] collation_name
  | COMMENT [=] 'string'
  | CONNECTION [=] 'connect_string'
  | DATA DIRECTORY [=] 'absolute path to directory'
  | DELAY_KEY_WRITE [=] {0 | 1}
  | INDEX DIRECTORY [=] 'absolute path to directory'
  | INSERT_METHOD [=] { NO | FIRST | LAST }
  | KEY_BLOCK_SIZE [=] value
  | MAX_ROWS [=] value
  | MIN_ROWS [=] value
  | PACK_KEYS [=] {0 | 1 | DEFAULT}
  | PASSWORD [=] 'string'
  | ROW_FORMAT [=] {DEFAULT|DYNAMIC|FIXED|COMPRESSED|REDUNDANT|COMPACT}
  | TABLESPACE tablespace_name [STORAGE {DISK|MEMORY|DEFAULT}]
  | UNION [=] (tbl_name[,tbl_name]...)

partition_options:
    PARTITION BY
        { [LINEAR] HASH(expr)
        | [LINEAR] KEY [ALGORITHM={1|2}] (column_list)
        | RANGE{(expr) | COLUMNS(column_list)}
        | LIST{(expr) | COLUMNS(column_list)} }
    [PARTITIONS num]
    [SUBPARTITION BY
        { [LINEAR] HASH(expr)
        | [LINEAR] KEY [ALGORITHM={1|2}] (column_list) }
      [SUBPARTITIONS num]
    ]
    [(partition_definition [, partition_definition] ...)]

partition_definition:
    PARTITION partition_name
        [VALUES 
            {LESS THAN {(expr | value_list) | MAXVALUE} 
            | 
            IN (value_list)}]
        [[STORAGE] ENGINE [=] engine_name]
        [COMMENT [=] 'comment_text' ]
        [DATA DIRECTORY [=] 'data_dir']
        [INDEX DIRECTORY [=] 'index_dir']
        [MAX_ROWS [=] max_number_of_rows]
        [MIN_ROWS [=] min_number_of_rows]
        [TABLESPACE [=] tablespace_name]
        [NODEGROUP [=] node_group_id]
        [(subpartition_definition [, subpartition_definition] ...)]

subpartition_definition:
    SUBPARTITION logical_name
        [[STORAGE] ENGINE [=] engine_name]
        [COMMENT [=] 'comment_text' ]
        [DATA DIRECTORY [=] 'data_dir']
        [INDEX DIRECTORY [=] 'index_dir']
        [MAX_ROWS [=] max_number_of_rows]
        [MIN_ROWS [=] min_number_of_rows]
        [TABLESPACE [=] tablespace_name]
        [NODEGROUP [=] node_group_id]

select_statement:
    [IGNORE | REPLACE] [AS] SELECT ...   (Some valid select statement)

CREATE TABLE creates a table with the given name. You must have the
CREATE privilege for the table.

Rules for permissible table names are given in
http://dev.mysql.com/doc/refman/5.5/en/identifiers.html. By default,
the table is created in the default database, using the InnoDB storage
engine. An error occurs if the table exists, if there is no default
database, or if the database does not exist.

URL: http://dev.mysql.com/doc/refman/5.5/en/create-table.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-table.html</field>
	</row>
	<row>
		<field name="help_topic_id">471</field>
		<field name="name">&gt;</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
&gt;

Greater than:

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT 2 &gt; 2;
        -&gt; 0
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">472</field>
		<field name="name">ANALYZE TABLE</field>
		<field name="help_category_id">21</field>
		<field name="description">Syntax:
ANALYZE [NO_WRITE_TO_BINLOG | LOCAL] TABLE
    tbl_name [, tbl_name] ...

ANALYZE TABLE analyzes and stores the key distribution for a table.
During the analysis, the table is locked with a read lock for InnoDB
and MyISAM. This statement works with InnoDB, NDB, and MyISAM tables.
For MyISAM tables, this statement is equivalent to using myisamchk
--analyze.

For more information on how the analysis works within InnoDB, see
http://dev.mysql.com/doc/refman/5.5/en/innodb-restrictions.html.

MySQL uses the stored key distribution to decide the order in which
tables should be joined when you perform a join on something other than
a constant. In addition, key distributions can be used when deciding
which indexes to use for a specific table within a query.

This statement requires SELECT and INSERT privileges for the table.

ANALYZE TABLE is supported for partitioned tables, and you can use
ALTER TABLE ... ANALYZE PARTITION to analyze one or more partitions;
for more information, see [HELP ALTER TABLE], and
http://dev.mysql.com/doc/refman/5.5/en/partitioning-maintenance.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/analyze-table.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/analyze-table.html</field>
	</row>
	<row>
		<field name="help_topic_id">473</field>
		<field name="name">MICROSECOND</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
MICROSECOND(expr)

Returns the microseconds from the time or datetime expression expr as a
number in the range from 0 to 999999.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT MICROSECOND('12:00:00.123456');
        -&gt; 123456
mysql&gt; SELECT MICROSECOND('2009-12-31 23:59:59.000010');
        -&gt; 10
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">474</field>
		<field name="name">CONSTRAINT</field>
		<field name="help_category_id">40</field>
		<field name="description">MySQL supports foreign keys, which let you cross-reference related data
across tables, and foreign key constraints, which help keep this
spread-out data consistent. The essential syntax for a foreign key
constraint definition in a CREATE TABLE or ALTER TABLE statement looks
like this:

[CONSTRAINT [symbol]] FOREIGN KEY
    [index_name] (index_col_name, ...)
    REFERENCES tbl_name (index_col_name,...)
    [ON DELETE reference_option]
    [ON UPDATE reference_option]

reference_option:
    RESTRICT | CASCADE | SET NULL | NO ACTION

URL: http://dev.mysql.com/doc/refman/5.5/en/create-table-foreign-keys.html

</field>
		<field name="example">CREATE TABLE product (
    category INT NOT NULL, id INT NOT NULL,
    price DECIMAL,
    PRIMARY KEY(category, id)
)   ENGINE=INNODB;

CREATE TABLE customer (
    id INT NOT NULL,
    PRIMARY KEY (id)
)   ENGINE=INNODB;

CREATE TABLE product_order (
    no INT NOT NULL AUTO_INCREMENT,
    product_category INT NOT NULL,
    product_id INT NOT NULL,
    customer_id INT NOT NULL,

    PRIMARY KEY(no),
    INDEX (product_category, product_id),
    INDEX (customer_id),

    FOREIGN KEY (product_category, product_id)
      REFERENCES product(category, id)
      ON UPDATE CASCADE ON DELETE RESTRICT,

    FOREIGN KEY (customer_id)
      REFERENCES customer(id)
)   ENGINE=INNODB;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-table-foreign-keys.html</field>
	</row>
	<row>
		<field name="help_topic_id">475</field>
		<field name="name">CREATE SERVER</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
CREATE SERVER server_name
    FOREIGN DATA WRAPPER wrapper_name
    OPTIONS (option [, option] ...)

option:
  { HOST character-literal
  | DATABASE character-literal
  | USER character-literal
  | PASSWORD character-literal
  | SOCKET character-literal
  | OWNER character-literal
  | PORT numeric-literal }

This statement creates the definition of a server for use with the
FEDERATED storage engine. The CREATE SERVER statement creates a new row
in the servers table in the mysql database. This statement requires the
SUPER privilege.

The server_name should be a unique reference to the server. Server
definitions are global within the scope of the server, it is not
possible to qualify the server definition to a specific database.
server_name has a maximum length of 64 characters (names longer than 64
characters are silently truncated), and is case insensitive. You may
specify the name as a quoted string.

The wrapper_name should be mysql, and may be quoted with single
quotation marks. Other values for wrapper_name are not currently
supported.

For each option you must specify either a character literal or numeric
literal. Character literals are UTF-8, support a maximum length of 64
characters and default to a blank (empty) string. String literals are
silently truncated to 64 characters. Numeric literals must be a number
between 0 and 9999, default value is 0.

*Note*: The OWNER option is currently not applied, and has no effect on
the ownership or operation of the server connection that is created.

The CREATE SERVER statement creates an entry in the mysql.servers table
that can later be used with the CREATE TABLE statement when creating a
FEDERATED table. The options that you specify will be used to populate
the columns in the mysql.servers table. The table columns are
Server_name, Host, Db, Username, Password, Port and Socket.

URL: http://dev.mysql.com/doc/refman/5.5/en/create-server.html

</field>
		<field name="example">CREATE SERVER s
FOREIGN DATA WRAPPER mysql
OPTIONS (USER 'Remote', HOST '192.168.1.106', DATABASE 'test');
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/create-server.html</field>
	</row>
	<row>
		<field name="help_topic_id">476</field>
		<field name="name">FIELD</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
FIELD(str,str1,str2,str3,...)

Returns the index (position) of str in the str1, str2, str3, ... list.
Returns 0 if str is not found.

If all arguments to FIELD() are strings, all arguments are compared as
strings. If all arguments are numbers, they are compared as numbers.
Otherwise, the arguments are compared as double.

If str is NULL, the return value is 0 because NULL fails equality
comparison with any value. FIELD() is the complement of ELT().

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT FIELD('ej', 'Hej', 'ej', 'Heja', 'hej', 'foo');
        -&gt; 2
mysql&gt; SELECT FIELD('fo', 'Hej', 'ej', 'Heja', 'hej', 'foo');
        -&gt; 0
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">477</field>
		<field name="name">MAKETIME</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
MAKETIME(hour,minute,second)

Returns a time value calculated from the hour, minute, and second
arguments.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT MAKETIME(12,15,30);
        -&gt; '12:15:30'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">478</field>
		<field name="name">CURDATE</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
CURDATE()

Returns the current date as a value in 'YYYY-MM-DD' or YYYYMMDD format,
depending on whether the function is used in a string or numeric
context.

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT CURDATE();
        -&gt; '2008-06-13'
mysql&gt; SELECT CURDATE() + 0;
        -&gt; 20080613
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">479</field>
		<field name="name">SET PASSWORD</field>
		<field name="help_category_id">10</field>
		<field name="description">Syntax:
SET PASSWORD [FOR user] =
    {
        PASSWORD('cleartext password')
      | OLD_PASSWORD('cleartext password')
      | 'encrypted password'
    }

The SET PASSWORD statement assigns a password to an existing MySQL user
account. When the read_only system variable is enabled, the SUPER
privilege is required to use SET PASSWORD, in addition to whatever
other privileges might be required.

If the password is specified using the PASSWORD() or OLD_PASSWORD()
function, the cleartext (unencrypted) password should be given as the
argument to the function, which hashes the password and returns the
encrypted password string. If the password is specified without using
either function, it should be the already encrypted password value as a
literal string. In all cases, the encrypted password string must be in
the format required by the authentication method used for the account.

With no FOR user clause, this statement sets the password for the
current user. (To see which account the server authenticated you as,
invoke the CURRENT_USER() function.) Any client who successfully
connects to the server using a nonanonymous account can change the
password for that account.

With a FOR user clause, this statement sets the password for the named
user. You must have the UPDATE privilege for the mysql database to do
this. The user account name uses the format described in
http://dev.mysql.com/doc/refman/5.5/en/account-names.html. The user
value should be given as 'user_name'@'host_name', where 'user_name' and
'host_name' are exactly as listed in the User and Host columns of the
mysql.user table row. (If you specify only a user name, a host name of
'%' is used.) For example, to set the password for an account with User
and Host column values of 'bob' and '%.example.org', write the
statement like this:

SET PASSWORD FOR 'bob'@'%.example.org' = PASSWORD('cleartext password');

That is equivalent to the following statements:

UPDATE mysql.user SET Password=PASSWORD('cleartext password')
  WHERE User='bob' AND Host='%.example.org';
FLUSH PRIVILEGES;

Another way to set the password is to use GRANT:

GRANT USAGE ON *.* TO 'bob'@'%.example.org' IDENTIFIED BY 'cleartext password';

The old_passwords system variable value determines the hashing method
used by PASSWORD(). If you specify the password using that function and
SET PASSWORD rejects the password as not being in the correct format,
it may be necessary to set old_passwords to change the hashing method.
For descriptions of the permitted values, see
http://dev.mysql.com/doc/refman/5.5/en/server-system-variables.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/set-password.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/set-password.html</field>
	</row>
	<row>
		<field name="help_topic_id">480</field>
		<field name="name">ALTER TABLESPACE</field>
		<field name="help_category_id">40</field>
		<field name="description">Syntax:
ALTER TABLESPACE tablespace_name
    {ADD|DROP} DATAFILE 'file_name'
    [INITIAL_SIZE [=] size]
    [WAIT]
    ENGINE [=] engine_name

This statement can be used either to add a new data file, or to drop a
data file from a tablespace.

The ADD DATAFILE variant enables you to specify an initial size using
an INITIAL_SIZE clause, where size is measured in bytes; the default
value is 128M (128 megabytes). You may optionally follow this integer
value with a one-letter abbreviation for an order of magnitude, similar
to those used in my.cnf. Generally, this is one of the letters M (for
megabytes) or G (for gigabytes).

*Note*: All MySQL Cluster Disk Data objects share the same namespace.
This means that each Disk Data object must be uniquely named (and not
merely each Disk Data object of a given type). For example, you cannot
have a tablespace and an data file with the same name, or an undo log
file and a tablespace with the same name.

On 32-bit systems, the maximum supported value for INITIAL_SIZE is 4G.
(Bug #29186)

INITIAL_SIZE is rounded, explicitly, as for CREATE TABLESPACE.

Once a data file has been created, its size cannot be changed; however,
you can add more data files to the tablespace using additional ALTER
TABLESPACE ... ADD DATAFILE statements.

Using DROP DATAFILE with ALTER TABLESPACE drops the data file
'file_name' from the tablespace. You cannot drop a data file from a
tablespace which is in use by any table; in other words, the data file
must be empty (no extents used). See
http://dev.mysql.com/doc/refman/5.5/en/mysql-cluster-disk-data-objects.
html. In addition, any data file to be dropped must previously have
been added to the tablespace with CREATE TABLESPACE or ALTER
TABLESPACE.

Both ALTER TABLESPACE ... ADD DATAFILE and ALTER TABLESPACE ... DROP
DATAFILE require an ENGINE clause which specifies the storage engine
used by the tablespace. Currently, the only accepted values for
engine_name are NDB and NDBCLUSTER.

WAIT is parsed but otherwise ignored, and so has no effect in MySQL
5.5. It is intended for future expansion.

When ALTER TABLESPACE ... ADD DATAFILE is used with ENGINE = NDB, a
data file is created on each Cluster data node. You can verify that the
data files were created and obtain information about them by querying
the INFORMATION_SCHEMA.FILES table. For example, the following query
shows all data files belonging to the tablespace named newts:

mysql&gt; SELECT LOGFILE_GROUP_NAME, FILE_NAME, EXTRA
    -&gt; FROM INFORMATION_SCHEMA.FILES
    -&gt; WHERE TABLESPACE_NAME = 'newts' AND FILE_TYPE = 'DATAFILE';
+--------------------+--------------+----------------+
| LOGFILE_GROUP_NAME | FILE_NAME    | EXTRA          |
+--------------------+--------------+----------------+
| lg_3               | newdata.dat  | CLUSTER_NODE=3 |
| lg_3               | newdata.dat  | CLUSTER_NODE=4 |
| lg_3               | newdata2.dat | CLUSTER_NODE=3 |
| lg_3               | newdata2.dat | CLUSTER_NODE=4 |
+--------------------+--------------+----------------+
2 rows in set (0.03 sec)

See http://dev.mysql.com/doc/refman/5.5/en/files-table.html.

ALTER TABLESPACE is useful only with Disk Data storage for MySQL
Cluster. See
http://dev.mysql.com/doc/refman/5.5/en/mysql-cluster-disk-data.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/alter-tablespace.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/alter-tablespace.html</field>
	</row>
	<row>
		<field name="help_topic_id">481</field>
		<field name="name">IF FUNCTION</field>
		<field name="help_category_id">7</field>
		<field name="description">Syntax:
IF(expr1,expr2,expr3)

If expr1 is TRUE (expr1 &lt;&gt; 0 and expr1 &lt;&gt; NULL) then IF() returns
expr2; otherwise it returns expr3. IF() returns a numeric or string
value, depending on the context in which it is used.

URL: http://dev.mysql.com/doc/refman/5.5/en/control-flow-functions.html

</field>
		<field name="example">mysql&gt; SELECT IF(1&gt;2,2,3);
        -&gt; 3
mysql&gt; SELECT IF(1&lt;2,'yes','no');
        -&gt; 'yes'
mysql&gt; SELECT IF(STRCMP('test','test1'),'no','yes');
        -&gt; 'no'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/control-flow-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">482</field>
		<field name="name">ENUM</field>
		<field name="help_category_id">23</field>
		<field name="description">ENUM('value1','value2',...) [CHARACTER SET charset_name] [COLLATE
collation_name]

An enumeration. A string object that can have only one value, chosen
from the list of values 'value1', 'value2', ..., NULL or the special ''
error value. ENUM values are represented internally as integers.

An ENUM column can have a maximum of 65,535 distinct elements. (The
practical limit is less than 3000.) A table can have no more than 255
unique element list definitions among its ENUM and SET columns
considered as a group. For more information on these limits, see
http://dev.mysql.com/doc/refman/5.5/en/limits-frm-file.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">483</field>
		<field name="name">DATABASE</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
DATABASE()

Returns the default (current) database name as a string in the utf8
character set. If there is no default database, DATABASE() returns
NULL. Within a stored routine, the default database is the database
that the routine is associated with, which is not necessarily the same
as the database that is the default in the calling context.

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example">mysql&gt; SELECT DATABASE();
        -&gt; 'test'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">484</field>
		<field name="name">POINTFROMWKB</field>
		<field name="help_category_id">33</field>
		<field name="description">PointFromWKB(wkb[,srid])

Constructs a POINT value using its WKB representation and SRID.

URL: http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/creating-spatial-values.html</field>
	</row>
	<row>
		<field name="help_topic_id">485</field>
		<field name="name">POWER</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
POWER(X,Y)

This is a synonym for POW().

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">486</field>
		<field name="name">ATAN</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
ATAN(X)

Returns the arc tangent of X, that is, the value whose tangent is X.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT ATAN(2);
        -&gt; 1.1071487177941
mysql&gt; SELECT ATAN(-2);
        -&gt; -1.1071487177941
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">487</field>
		<field name="name">STRCMP</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
STRCMP(expr1,expr2)

STRCMP() returns 0 if the strings are the same, -1 if the first
argument is smaller than the second according to the current sort
order, and 1 otherwise.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-comparison-functions.html

</field>
		<field name="example">mysql&gt; SELECT STRCMP('text', 'text2');
        -&gt; -1
mysql&gt; SELECT STRCMP('text2', 'text');
        -&gt; 1
mysql&gt; SELECT STRCMP('text', 'text');
        -&gt; 0
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-comparison-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">488</field>
		<field name="name">INSERT DELAYED</field>
		<field name="help_category_id">28</field>
		<field name="description">Syntax:
INSERT DELAYED ...

The DELAYED option for the INSERT statement is a MySQL extension to
standard SQL that is very useful if you have clients that cannot or
need not wait for the INSERT to complete. This is a common situation
when you use MySQL for logging and you also periodically run SELECT and
UPDATE statements that take a long time to complete.

When a client uses INSERT DELAYED, it gets an okay from the server at
once, and the row is queued to be inserted when the table is not in use
by any other thread.

Another major benefit of using INSERT DELAYED is that inserts from many
clients are bundled together and written in one block. This is much
faster than performing many separate inserts.

Note that INSERT DELAYED is slower than a normal INSERT if the table is
not otherwise in use. There is also the additional overhead for the
server to handle a separate thread for each table for which there are
delayed rows. This means that you should use INSERT DELAYED only when
you are really sure that you need it.

The queued rows are held only in memory until they are inserted into
the table. This means that if you terminate mysqld forcibly (for
example, with kill -9) or if mysqld dies unexpectedly, any queued rows
that have not been written to disk are lost.

There are some constraints on the use of DELAYED:

o INSERT DELAYED works only with MyISAM, MEMORY, ARCHIVE, and BLACKHOLE
  tables. For engines that do not support DELAYED, an error occurs.

o An error occurs for INSERT DELAYED if used with a table that has been
  locked with LOCK TABLES because the insert must be handled by a
  separate thread, not by the session that holds the lock.

o For MyISAM tables, if there are no free blocks in the middle of the
  data file, concurrent SELECT and INSERT statements are supported.
  Under these circumstances, you very seldom need to use INSERT DELAYED
  with MyISAM.

o INSERT DELAYED should be used only for INSERT statements that specify
  value lists. The server ignores DELAYED for INSERT ... SELECT or
  INSERT ... ON DUPLICATE KEY UPDATE statements.

o Because the INSERT DELAYED statement returns immediately, before the
  rows are inserted, you cannot use LAST_INSERT_ID() to get the
  AUTO_INCREMENT value that the statement might generate.

o DELAYED rows are not visible to SELECT statements until they actually
  have been inserted.

o Prior to MySQL 5.5.7, INSERT DELAYED was treated as a normal INSERT
  if the statement inserted multiple rows, binary logging was enabled,
  and the global logging format was statement-based (that is, whenever
  binlog_format was set to STATEMENT). Beginning with MySQL 5.5.7,
  INSERT DELAYED is always handled as a simple INSERT (that is, without
  the DELAYED option) whenever the value of binlog_format is STATEMENT
  or MIXED. (In the latter case, the statement no longer triggers a
  switch to row-based logging, and so is logged using the
  statement-based format.)

  This does not apply when using row-based binary logging mode
  (binlog_format set to ROW), in which INSERT DELAYED statements are
  always executed using the DELAYED option as specified, and logged as
  row-update events.

o DELAYED is ignored on slave replication servers, so that INSERT
  DELAYED is treated as a normal INSERT on slaves. This is because
  DELAYED could cause the slave to have different data than the master.

o Pending INSERT DELAYED statements are lost if a table is write locked
  and ALTER TABLE is used to modify the table structure.

o INSERT DELAYED is not supported for views.

o INSERT DELAYED is not supported for partitioned tables.

URL: http://dev.mysql.com/doc/refman/5.5/en/insert-delayed.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/insert-delayed.html</field>
	</row>
	<row>
		<field name="help_topic_id">489</field>
		<field name="name">SHOW PROFILE</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW PROFILE [type [, type] ... ]
    [FOR QUERY n]
    [LIMIT row_count [OFFSET offset]]

type:
    ALL
  | BLOCK IO
  | CONTEXT SWITCHES
  | CPU
  | IPC
  | MEMORY
  | PAGE FAULTS
  | SOURCE
  | SWAPS

The SHOW PROFILE and SHOW PROFILES statements display profiling
information that indicates resource usage for statements executed
during the course of the current session.

Profiling is controlled by the profiling session variable, which has a
default value of 0 (OFF). Profiling is enabled by setting profiling to
1 or ON:

mysql&gt; SET profiling = 1;

SHOW PROFILES displays a list of the most recent statements sent to the
server. The size of the list is controlled by the
profiling_history_size session variable, which has a default value of
15. The maximum value is 100. Setting the value to 0 has the practical
effect of disabling profiling.

All statements are profiled except SHOW PROFILE and SHOW PROFILES, so
you will find neither of those statements in the profile list.
Malformed statements are profiled. For example, SHOW PROFILING is an
illegal statement, and a syntax error occurs if you try to execute it,
but it will show up in the profiling list.

SHOW PROFILE displays detailed information about a single statement.
Without the FOR QUERY n clause, the output pertains to the most
recently executed statement. If FOR QUERY n is included, SHOW PROFILE
displays information for statement n. The values of n correspond to the
Query_ID values displayed by SHOW PROFILES.

The LIMIT row_count clause may be given to limit the output to
row_count rows. If LIMIT is given, OFFSET offset may be added to begin
the output offset rows into the full set of rows.

By default, SHOW PROFILE displays Status and Duration columns. The
Status values are like the State values displayed by SHOW PROCESSLIST,
although there might be some minor differences in interpretion for the
two statements for some status values (see
http://dev.mysql.com/doc/refman/5.5/en/thread-information.html).

Optional type values may be specified to display specific additional
types of information:

o ALL displays all information

o BLOCK IO displays counts for block input and output operations

o CONTEXT SWITCHES displays counts for voluntary and involuntary
  context switches

o CPU displays user and system CPU usage times

o IPC displays counts for messages sent and received

o MEMORY is not currently implemented

o PAGE FAULTS displays counts for major and minor page faults

o SOURCE displays the names of functions from the source code, together
  with the name and line number of the file in which the function
  occurs

o SWAPS displays swap counts

Profiling is enabled per session. When a session ends, its profiling
information is lost.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-profile.html

</field>
		<field name="example">mysql&gt; SELECT @@profiling;
+-------------+
| @@profiling |
+-------------+
|           0 |
+-------------+
1 row in set (0.00 sec)

mysql&gt; SET profiling = 1;
Query OK, 0 rows affected (0.00 sec)

mysql&gt; DROP TABLE IF EXISTS t1;
Query OK, 0 rows affected, 1 warning (0.00 sec)

mysql&gt; CREATE TABLE T1 (id INT);
Query OK, 0 rows affected (0.01 sec)

mysql&gt; SHOW PROFILES;
+----------+----------+--------------------------+
| Query_ID | Duration | Query                    |
+----------+----------+--------------------------+
|        0 | 0.000088 | SET PROFILING = 1        |
|        1 | 0.000136 | DROP TABLE IF EXISTS t1  |
|        2 | 0.011947 | CREATE TABLE t1 (id INT) |
+----------+----------+--------------------------+
3 rows in set (0.00 sec)

mysql&gt; SHOW PROFILE;
+----------------------+----------+
| Status               | Duration |
+----------------------+----------+
| checking permissions | 0.000040 |
| creating table       | 0.000056 |
| After create         | 0.011363 |
| query end            | 0.000375 |
| freeing items        | 0.000089 |
| logging slow query   | 0.000019 |
| cleaning up          | 0.000005 |
+----------------------+----------+
7 rows in set (0.00 sec)

mysql&gt; SHOW PROFILE FOR QUERY 1;
+--------------------+----------+
| Status             | Duration |
+--------------------+----------+
| query end          | 0.000107 |
| freeing items      | 0.000008 |
| logging slow query | 0.000015 |
| cleaning up        | 0.000006 |
+--------------------+----------+
4 rows in set (0.00 sec)

mysql&gt; SHOW PROFILE CPU FOR QUERY 2;
+----------------------+----------+----------+------------+
| Status               | Duration | CPU_user | CPU_system |
+----------------------+----------+----------+------------+
| checking permissions | 0.000040 | 0.000038 |   0.000002 |
| creating table       | 0.000056 | 0.000028 |   0.000028 |
| After create         | 0.011363 | 0.000217 |   0.001571 |
| query end            | 0.000375 | 0.000013 |   0.000028 |
| freeing items        | 0.000089 | 0.000010 |   0.000014 |
| logging slow query   | 0.000019 | 0.000009 |   0.000010 |
| cleaning up          | 0.000005 | 0.000003 |   0.000002 |
+----------------------+----------+----------+------------+
7 rows in set (0.00 sec)
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-profile.html</field>
	</row>
	<row>
		<field name="help_topic_id">490</field>
		<field name="name">SHOW PROCEDURE CODE</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW PROCEDURE CODE proc_name

This statement is a MySQL extension that is available only for servers
that have been built with debugging support. It displays a
representation of the internal implementation of the named stored
procedure. A similar statement, SHOW FUNCTION CODE, displays
information about stored functions (see [HELP SHOW FUNCTION CODE]).

Both statements require that you be the owner of the routine or have
SELECT access to the mysql.proc table.

If the named routine is available, each statement produces a result
set. Each row in the result set corresponds to one &quot;instruction&quot; in the
routine. The first column is Pos, which is an ordinal number beginning
with 0. The second column is Instruction, which contains an SQL
statement (usually changed from the original source), or a directive
which has meaning only to the stored-routine handler.

URL: http://dev.mysql.com/doc/refman/5.5/en/show-procedure-code.html

</field>
		<field name="example">mysql&gt; DELIMITER //
mysql&gt; CREATE PROCEDURE p1 ()
    -&gt; BEGIN
    -&gt;   DECLARE fanta INT DEFAULT 55;
    -&gt;   DROP TABLE t2;
    -&gt;   LOOP
    -&gt;     INSERT INTO t3 VALUES (fanta);
    -&gt;     END LOOP;
    -&gt;   END//
Query OK, 0 rows affected (0.00 sec)

mysql&gt; SHOW PROCEDURE CODE p1//
+-----+----------------------------------------+
| Pos | Instruction                            |
+-----+----------------------------------------+
|   0 | set fanta@0 55                         |
|   1 | stmt 9 &quot;DROP TABLE t2&quot;                 |
|   2 | stmt 5 &quot;INSERT INTO t3 VALUES (fanta)&quot; |
|   3 | jump 2                                 |
+-----+----------------------------------------+
4 rows in set (0.00 sec)
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-procedure-code.html</field>
	</row>
	<row>
		<field name="help_topic_id">491</field>
		<field name="name">MEDIUMTEXT</field>
		<field name="help_category_id">23</field>
		<field name="description">MEDIUMTEXT [CHARACTER SET charset_name] [COLLATE collation_name]

A TEXT column with a maximum length of 16,777,215 (224 - 1) characters.
The effective maximum length is less if the value contains multi-byte
characters. Each MEDIUMTEXT value is stored using a 3-byte length
prefix that indicates the number of bytes in the value.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">492</field>
		<field name="name">LN</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
LN(X)

Returns the natural logarithm of X; that is, the base-e logarithm of X.
If X is less than or equal to 0, then NULL is returned.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT LN(2);
        -&gt; 0.69314718055995
mysql&gt; SELECT LN(-2);
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">493</field>
		<field name="name">RETURN</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
RETURN expr

The RETURN statement terminates execution of a stored function and
returns the value expr to the function caller. There must be at least
one RETURN statement in a stored function. There may be more than one
if the function has multiple exit points.

This statement is not used in stored procedures, triggers, or events.
The LEAVE statement can be used to exit a stored program of those
types.

URL: http://dev.mysql.com/doc/refman/5.5/en/return.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/return.html</field>
	</row>
	<row>
		<field name="help_topic_id">494</field>
		<field name="name">SHOW COLLATION</field>
		<field name="help_category_id">27</field>
		<field name="description">Syntax:
SHOW COLLATION
    [LIKE 'pattern' | WHERE expr]

This statement lists collations supported by the server. By default,
the output from SHOW COLLATION includes all available collations. The
LIKE clause, if present, indicates which collation names to match. The
WHERE clause can be given to select rows using more general conditions,
as discussed in
http://dev.mysql.com/doc/refman/5.5/en/extended-show.html. For example:

mysql&gt; SHOW COLLATION LIKE 'latin1%';
+-------------------+---------+----+---------+----------+---------+
| Collation         | Charset | Id | Default | Compiled | Sortlen |
+-------------------+---------+----+---------+----------+---------+
| latin1_german1_ci | latin1  |  5 |         |          |       0 |
| latin1_swedish_ci | latin1  |  8 | Yes     | Yes      |       0 |
| latin1_danish_ci  | latin1  | 15 |         |          |       0 |
| latin1_german2_ci | latin1  | 31 |         | Yes      |       2 |
| latin1_bin        | latin1  | 47 |         | Yes      |       0 |
| latin1_general_ci | latin1  | 48 |         |          |       0 |
| latin1_general_cs | latin1  | 49 |         |          |       0 |
| latin1_spanish_ci | latin1  | 94 |         |          |       0 |
+-------------------+---------+----+---------+----------+---------+

URL: http://dev.mysql.com/doc/refman/5.5/en/show-collation.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/show-collation.html</field>
	</row>
	<row>
		<field name="help_topic_id">495</field>
		<field name="name">LOG</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
LOG(X), LOG(B,X)

If called with one parameter, this function returns the natural
logarithm of X. If X is less than or equal to 0, then NULL is returned.

The inverse of this function (when called with a single argument) is
the EXP() function.

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT LOG(2);
        -&gt; 0.69314718055995
mysql&gt; SELECT LOG(-2);
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">496</field>
		<field name="name">SET SQL_LOG_BIN</field>
		<field name="help_category_id">8</field>
		<field name="description">Syntax:
SET sql_log_bin = {0|1}

The sql_log_bin variable controls whether logging to the binary log is
done. The default value is 1 (do logging). To change logging for the
current session, change the session value of this variable. The session
user must have the SUPER privilege to set this variable.

Beginning with MySQL 5.5.5, it is no longer possible to set
@@session.sql_log_bin within a transaction or subquery. (Bug #53437)

URL: http://dev.mysql.com/doc/refman/5.5/en/set-sql-log-bin.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/set-sql-log-bin.html</field>
	</row>
	<row>
		<field name="help_topic_id">497</field>
		<field name="name">!=</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
&lt;&gt;, !=

Not equal:

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT '.01' &lt;&gt; '0.01';
        -&gt; 1
mysql&gt; SELECT .01 &lt;&gt; '0.01';
        -&gt; 0
mysql&gt; SELECT 'zapp' &lt;&gt; 'zappp';
        -&gt; 1
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">498</field>
		<field name="name">WHILE</field>
		<field name="help_category_id">24</field>
		<field name="description">Syntax:
[begin_label:] WHILE search_condition DO
    statement_list
END WHILE [end_label]

The statement list within a WHILE statement is repeated as long as the
search_condition expression is true. statement_list consists of one or
more SQL statements, each terminated by a semicolon (;) statement
delimiter.

A WHILE statement can be labeled. For the rules regarding label use,
see [HELP labels].

URL: http://dev.mysql.com/doc/refman/5.5/en/while.html

</field>
		<field name="example">CREATE PROCEDURE dowhile()
BEGIN
  DECLARE v1 INT DEFAULT 5;

  WHILE v1 &gt; 0 DO
    ...
    SET v1 = v1 - 1;
  END WHILE;
END;
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/while.html</field>
	</row>
	<row>
		<field name="help_topic_id">499</field>
		<field name="name">AES_DECRYPT</field>
		<field name="help_category_id">12</field>
		<field name="description">Syntax:
AES_DECRYPT(crypt_str,key_str)

This function decrypts data using the official AES (Advanced Encryption
Standard) algorithm. For more information, see the description of
AES_ENCRYPT().

URL: http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/encryption-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">500</field>
		<field name="name">DAYNAME</field>
		<field name="help_category_id">32</field>
		<field name="description">Syntax:
DAYNAME(date)

Returns the name of the weekday for date. The language used for the
name is controlled by the value of the lc_time_names system variable
(http://dev.mysql.com/doc/refman/5.5/en/locale-support.html).

URL: http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html

</field>
		<field name="example">mysql&gt; SELECT DAYNAME('2007-02-03');
        -&gt; 'Saturday'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/date-and-time-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">501</field>
		<field name="name">COERCIBILITY</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
COERCIBILITY(str)

Returns the collation coercibility value of the string argument.

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example">mysql&gt; SELECT COERCIBILITY('abc' COLLATE latin1_swedish_ci);
        -&gt; 0
mysql&gt; SELECT COERCIBILITY(USER());
        -&gt; 3
mysql&gt; SELECT COERCIBILITY('abc');
        -&gt; 4
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">502</field>
		<field name="name">INT</field>
		<field name="help_category_id">23</field>
		<field name="description">INT[(M)] [UNSIGNED] [ZEROFILL]

A normal-size integer. The signed range is -2147483648 to 2147483647.
The unsigned range is 0 to 4294967295.

URL: http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html

</field>
		<field name="example"></field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/numeric-type-overview.html</field>
	</row>
	<row>
		<field name="help_topic_id">503</field>
		<field name="name">GLENGTH</field>
		<field name="help_category_id">13</field>
		<field name="description">GLength(ls)

Returns as a double-precision number the length of the LineString value
ls in its associated spatial reference.

URL: http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html

</field>
		<field name="example">mysql&gt; SET @ls = 'LineString(1 1,2 2,3 3)';
mysql&gt; SELECT GLength(GeomFromText(@ls));
+----------------------------+
| GLength(GeomFromText(@ls)) |
+----------------------------+
|            2.8284271247462 |
+----------------------------+
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/geometry-property-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">504</field>
		<field name="name">RADIANS</field>
		<field name="help_category_id">4</field>
		<field name="description">Syntax:
RADIANS(X)

Returns the argument X, converted from degrees to radians. (Note that
π radians equals 180 degrees.)

URL: http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html

</field>
		<field name="example">mysql&gt; SELECT RADIANS(90);
        -&gt; 1.5707963267949
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/mathematical-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">505</field>
		<field name="name">COLLATION</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
COLLATION(str)

Returns the collation of the string argument.

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example">mysql&gt; SELECT COLLATION('abc');
        -&gt; 'latin1_swedish_ci'
mysql&gt; SELECT COLLATION(_utf8'abc');
        -&gt; 'utf8_general_ci'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">506</field>
		<field name="name">COALESCE</field>
		<field name="help_category_id">19</field>
		<field name="description">Syntax:
COALESCE(value,...)

Returns the first non-NULL value in the list, or NULL if there are no
non-NULL values.

URL: http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html

</field>
		<field name="example">mysql&gt; SELECT COALESCE(NULL,1);
        -&gt; 1
mysql&gt; SELECT COALESCE(NULL,NULL,NULL);
        -&gt; NULL
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/comparison-operators.html</field>
	</row>
	<row>
		<field name="help_topic_id">507</field>
		<field name="name">VERSION</field>
		<field name="help_category_id">17</field>
		<field name="description">Syntax:
VERSION()

Returns a string that indicates the MySQL server version. The string
uses the utf8 character set. The value might have a suffix in addition
to the version number. See the description of the version system
variable in
http://dev.mysql.com/doc/refman/5.5/en/server-system-variables.html.

URL: http://dev.mysql.com/doc/refman/5.5/en/information-functions.html

</field>
		<field name="example">mysql&gt; SELECT VERSION();
        -&gt; '5.5.32-standard'
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/information-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">508</field>
		<field name="name">MAKE_SET</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
MAKE_SET(bits,str1,str2,...)

Returns a set value (a string containing substrings separated by &quot;,&quot;
characters) consisting of the strings that have the corresponding bit
in bits set. str1 corresponds to bit 0, str2 to bit 1, and so on. NULL
values in str1, str2, ... are not appended to the result.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT MAKE_SET(1,'a','b','c');
        -&gt; 'a'
mysql&gt; SELECT MAKE_SET(1 | 4,'hello','nice','world');
        -&gt; 'hello,world'
mysql&gt; SELECT MAKE_SET(1 | 4,'hello','nice',NULL,'world');
        -&gt; 'hello'
mysql&gt; SELECT MAKE_SET(0,'a','b','c');
        -&gt; ''
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	<row>
		<field name="help_topic_id">509</field>
		<field name="name">FIND_IN_SET</field>
		<field name="help_category_id">38</field>
		<field name="description">Syntax:
FIND_IN_SET(str,strlist)

Returns a value in the range of 1 to N if the string str is in the
string list strlist consisting of N substrings. A string list is a
string composed of substrings separated by &quot;,&quot; characters. If the first
argument is a constant string and the second is a column of type SET,
the FIND_IN_SET() function is optimized to use bit arithmetic. Returns
0 if str is not in strlist or if strlist is the empty string. Returns
NULL if either argument is NULL. This function does not work properly
if the first argument contains a comma (&quot;,&quot;) character.

URL: http://dev.mysql.com/doc/refman/5.5/en/string-functions.html

</field>
		<field name="example">mysql&gt; SELECT FIND_IN_SET('b','a,b,c,d');
        -&gt; 2
</field>
		<field name="url">http://dev.mysql.com/doc/refman/5.5/en/string-functions.html</field>
	</row>
	</table_data>
	<table_structure name="host">
		<field Field="Host" Type="char(60)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Db" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Select_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Insert_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Update_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Delete_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Drop_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Grant_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="References_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Index_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Alter_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_tmp_table_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Lock_tables_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_view_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Show_view_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_routine_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Alter_routine_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Execute_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Trigger_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<key Table="host" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Host" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="host" Non_unique="0" Key_name="PRIMARY" Seq_in_index="2" Column_name="Db" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="host" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="110056715893866495" Index_length="2048" Data_free="0" Create_time="2013-06-14 00:07:19" Update_time="2013-06-14 00:07:19" Collation="utf8_bin" Create_options="" Comment="Host privileges;  Merged with database privileges" />
	</table_structure>
	<table_data name="host">
	</table_data>
	<table_structure name="ndb_binlog_index">
		<field Field="Position" Type="bigint(20) unsigned" Null="NO" Key="" Extra="" Comment="" />
		<field Field="File" Type="varchar(255)" Null="NO" Key="" Extra="" Comment="" />
		<field Field="epoch" Type="bigint(20) unsigned" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="inserts" Type="bigint(20) unsigned" Null="NO" Key="" Extra="" Comment="" />
		<field Field="updates" Type="bigint(20) unsigned" Null="NO" Key="" Extra="" Comment="" />
		<field Field="deletes" Type="bigint(20) unsigned" Null="NO" Key="" Extra="" Comment="" />
		<field Field="schemaops" Type="bigint(20) unsigned" Null="NO" Key="" Extra="" Comment="" />
		<key Table="ndb_binlog_index" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="epoch" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="ndb_binlog_index" Engine="MyISAM" Version="10" Row_format="Dynamic" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="281474976710655" Index_length="1024" Data_free="0" Create_time="2013-06-14 00:06:55" Update_time="2013-06-14 00:06:55" Collation="latin1_swedish_ci" Create_options="" Comment="" />
	</table_structure>
	<table_data name="ndb_binlog_index">
	</table_data>
	<table_structure name="plugin">
		<field Field="name" Type="varchar(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="dl" Type="varchar(128)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<key Table="plugin" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="name" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="plugin" Engine="MyISAM" Version="10" Row_format="Dynamic" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="281474976710655" Index_length="1024" Data_free="0" Create_time="2013-06-14 00:07:15" Update_time="2013-06-14 00:07:15" Collation="utf8_general_ci" Create_options="" Comment="MySQL plugins" />
	</table_structure>
	<table_data name="plugin">
	</table_data>
	<table_structure name="proc">
		<field Field="db" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="name" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="type" Type="enum('FUNCTION','PROCEDURE')" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="specific_name" Type="char(64)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="language" Type="enum('SQL')" Null="NO" Key="" Default="SQL" Extra="" Comment="" />
		<field Field="sql_data_access" Type="enum('CONTAINS_SQL','NO_SQL','READS_SQL_DATA','MODIFIES_SQL_DATA')" Null="NO" Key="" Default="CONTAINS_SQL" Extra="" Comment="" />
		<field Field="is_deterministic" Type="enum('YES','NO')" Null="NO" Key="" Default="NO" Extra="" Comment="" />
		<field Field="security_type" Type="enum('INVOKER','DEFINER')" Null="NO" Key="" Default="DEFINER" Extra="" Comment="" />
		<field Field="param_list" Type="blob" Null="NO" Key="" Extra="" Comment="" />
		<field Field="returns" Type="longblob" Null="NO" Key="" Extra="" Comment="" />
		<field Field="body" Type="longblob" Null="NO" Key="" Extra="" Comment="" />
		<field Field="definer" Type="char(77)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="created" Type="timestamp" Null="NO" Key="" Default="CURRENT_TIMESTAMP" Extra="on update CURRENT_TIMESTAMP" Comment="" />
		<field Field="modified" Type="timestamp" Null="NO" Key="" Default="0000-00-00 00:00:00" Extra="" Comment="" />
		<field Field="sql_mode" Type="set('REAL_AS_FLOAT','PIPES_AS_CONCAT','ANSI_QUOTES','IGNORE_SPACE','NOT_USED','ONLY_FULL_GROUP_BY','NO_UNSIGNED_SUBTRACTION','NO_DIR_IN_CREATE','POSTGRESQL','ORACLE','MSSQL','DB2','MAXDB','NO_KEY_OPTIONS','NO_TABLE_OPTIONS','NO_FIELD_OPTIONS','MYSQL323','MYSQL40','ANSI','NO_AUTO_VALUE_ON_ZERO','NO_BACKSLASH_ESCAPES','STRICT_TRANS_TABLES','STRICT_ALL_TABLES','NO_ZERO_IN_DATE','NO_ZERO_DATE','INVALID_DATES','ERROR_FOR_DIVISION_BY_ZERO','TRADITIONAL','NO_AUTO_CREATE_USER','HIGH_NOT_PRECEDENCE','NO_ENGINE_SUBSTITUTION','PAD_CHAR_TO_FULL_LENGTH')" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="comment" Type="text" Null="NO" Key="" Extra="" Comment="" />
		<field Field="character_set_client" Type="char(32)" Null="YES" Key="" Extra="" Comment="" />
		<field Field="collation_connection" Type="char(32)" Null="YES" Key="" Extra="" Comment="" />
		<field Field="db_collation" Type="char(32)" Null="YES" Key="" Extra="" Comment="" />
		<field Field="body_utf8" Type="longblob" Null="YES" Key="" Extra="" Comment="" />
		<key Table="proc" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="db" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="proc" Non_unique="0" Key_name="PRIMARY" Seq_in_index="2" Column_name="name" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="proc" Non_unique="0" Key_name="PRIMARY" Seq_in_index="3" Column_name="type" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="proc" Engine="MyISAM" Version="10" Row_format="Dynamic" Rows="0" Avg_row_length="0" Data_length="320" Max_data_length="281474976710655" Index_length="4096" Data_free="320" Create_time="2013-06-14 00:07:18" Update_time="2013-06-14 00:07:20" Collation="utf8_general_ci" Create_options="" Comment="Stored Procedures" />
	</table_structure>
	<table_data name="proc">
	</table_data>
	<table_structure name="procs_priv">
		<field Field="Host" Type="char(60)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Db" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="User" Type="char(16)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Routine_name" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Routine_type" Type="enum('FUNCTION','PROCEDURE')" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="Grantor" Type="char(77)" Null="NO" Key="MUL" Default="" Extra="" Comment="" />
		<field Field="Proc_priv" Type="set('Execute','Alter Routine','Grant')" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="Timestamp" Type="timestamp" Null="NO" Key="" Default="CURRENT_TIMESTAMP" Extra="on update CURRENT_TIMESTAMP" Comment="" />
		<key Table="procs_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Host" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="procs_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="2" Column_name="Db" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="procs_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="3" Column_name="User" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="procs_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="4" Column_name="Routine_name" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="procs_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="5" Column_name="Routine_type" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="procs_priv" Non_unique="1" Key_name="Grantor" Seq_in_index="1" Column_name="Grantor" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="procs_priv" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="239253730204057599" Index_length="4096" Data_free="0" Create_time="2013-06-14 00:07:17" Update_time="2013-06-14 00:07:17" Check_time="2013-06-14 00:07:17" Collation="utf8_bin" Create_options="" Comment="Procedure privileges" />
	</table_structure>
	<table_data name="procs_priv">
	</table_data>
	<table_structure name="proxies_priv">
		<field Field="Host" Type="char(60)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="User" Type="char(16)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Proxied_host" Type="char(60)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Proxied_user" Type="char(16)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="With_grant" Type="tinyint(1)" Null="NO" Key="" Default="0" Extra="" Comment="" />
		<field Field="Grantor" Type="char(77)" Null="NO" Key="MUL" Default="" Extra="" Comment="" />
		<field Field="Timestamp" Type="timestamp" Null="NO" Key="" Default="CURRENT_TIMESTAMP" Extra="on update CURRENT_TIMESTAMP" Comment="" />
		<key Table="proxies_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Host" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="proxies_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="2" Column_name="User" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="proxies_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="3" Column_name="Proxied_host" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="proxies_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="4" Column_name="Proxied_user" Collation="A" Cardinality="2" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="proxies_priv" Non_unique="1" Key_name="Grantor" Seq_in_index="1" Column_name="Grantor" Collation="A" Cardinality="1" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="proxies_priv" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="2" Avg_row_length="693" Data_length="1386" Max_data_length="195062158860484607" Index_length="5120" Data_free="0" Create_time="2013-06-14 00:06:56" Update_time="2013-06-14 00:06:56" Check_time="2013-06-14 00:06:56" Collation="utf8_bin" Create_options="" Comment="User proxy privileges" />
	</table_structure>
	<table_data name="proxies_priv">
	<row>
		<field name="Host">localhost</field>
		<field name="User">root</field>
		<field name="Proxied_host"></field>
		<field name="Proxied_user"></field>
		<field name="With_grant">1</field>
		<field name="Grantor"></field>
		<field name="Timestamp">2013-06-14 00:06:56</field>
	</row>
	<row>
		<field name="Host">onedirac</field>
		<field name="User">root</field>
		<field name="Proxied_host"></field>
		<field name="Proxied_user"></field>
		<field name="With_grant">1</field>
		<field name="Grantor"></field>
		<field name="Timestamp">2013-06-14 00:06:56</field>
	</row>
	</table_data>
	<table_structure name="servers">
		<field Field="Server_name" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Host" Type="char(64)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="Db" Type="char(64)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="Username" Type="char(64)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="Password" Type="char(64)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="Port" Type="int(4)" Null="NO" Key="" Default="0" Extra="" Comment="" />
		<field Field="Socket" Type="char(64)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="Wrapper" Type="char(64)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="Owner" Type="char(64)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<key Table="servers" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Server_name" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="servers" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="433752939111120895" Index_length="1024" Data_free="0" Create_time="2013-06-14 00:06:54" Update_time="2013-06-14 00:06:54" Collation="utf8_general_ci" Create_options="" Comment="MySQL Foreign Servers table" />
	</table_structure>
	<table_data name="servers">
	</table_data>
	<table_structure name="tables_priv">
		<field Field="Host" Type="char(60)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Db" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="User" Type="char(16)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Table_name" Type="char(64)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Grantor" Type="char(77)" Null="NO" Key="MUL" Default="" Extra="" Comment="" />
		<field Field="Timestamp" Type="timestamp" Null="NO" Key="" Default="CURRENT_TIMESTAMP" Extra="on update CURRENT_TIMESTAMP" Comment="" />
		<field Field="Table_priv" Type="set('Select','Insert','Update','Delete','Create','Drop','Grant','References','Index','Alter','Create View','Show view','Trigger')" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="Column_priv" Type="set('Select','Insert','Update','References')" Null="NO" Key="" Default="" Extra="" Comment="" />
		<key Table="tables_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Host" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="tables_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="2" Column_name="Db" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="tables_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="3" Column_name="User" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="tables_priv" Non_unique="0" Key_name="PRIMARY" Seq_in_index="4" Column_name="Table_name" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="tables_priv" Non_unique="1" Key_name="Grantor" Seq_in_index="1" Column_name="Grantor" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="tables_priv" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="239535205180768255" Index_length="4096" Data_free="0" Create_time="2013-06-14 00:07:12" Update_time="2013-06-14 00:07:12" Check_time="2013-06-14 00:07:12" Collation="utf8_bin" Create_options="" Comment="Table privileges" />
	</table_structure>
	<table_data name="tables_priv">
	</table_data>
	<table_structure name="time_zone">
		<field Field="Time_zone_id" Type="int(10) unsigned" Null="NO" Key="PRI" Extra="auto_increment" Comment="" />
		<field Field="Use_leap_seconds" Type="enum('Y','N')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<key Table="time_zone" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Time_zone_id" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="time_zone" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="1970324836974591" Index_length="1024" Data_free="0" Auto_increment="1" Create_time="2013-06-14 00:06:54" Update_time="2013-06-14 00:06:54" Collation="utf8_general_ci" Create_options="" Comment="Time zones" />
	</table_structure>
	<table_data name="time_zone">
	</table_data>
	<table_structure name="time_zone_leap_second">
		<field Field="Transition_time" Type="bigint(20)" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="Correction" Type="int(11)" Null="NO" Key="" Extra="" Comment="" />
		<key Table="time_zone_leap_second" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Transition_time" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="time_zone_leap_second" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="3659174697238527" Index_length="1024" Data_free="0" Create_time="2013-06-14 00:06:54" Update_time="2013-06-14 00:06:54" Collation="utf8_general_ci" Create_options="" Comment="Leap seconds information for time zones" />
	</table_structure>
	<table_data name="time_zone_leap_second">
	</table_data>
	<table_structure name="time_zone_name">
		<field Field="Name" Type="char(64)" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="Time_zone_id" Type="int(10) unsigned" Null="NO" Key="" Extra="" Comment="" />
		<key Table="time_zone_name" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Name" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="time_zone_name" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="55450570411999231" Index_length="1024" Data_free="0" Create_time="2013-06-14 00:06:54" Update_time="2013-06-14 00:06:54" Collation="utf8_general_ci" Create_options="" Comment="Time zone names" />
	</table_structure>
	<table_data name="time_zone_name">
	</table_data>
	<table_structure name="time_zone_transition">
		<field Field="Time_zone_id" Type="int(10) unsigned" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="Transition_time" Type="bigint(20)" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="Transition_type_id" Type="int(10) unsigned" Null="NO" Key="" Extra="" Comment="" />
		<key Table="time_zone_transition" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Time_zone_id" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="time_zone_transition" Non_unique="0" Key_name="PRIMARY" Seq_in_index="2" Column_name="Transition_time" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="time_zone_transition" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="4785074604081151" Index_length="1024" Data_free="0" Create_time="2013-06-14 00:06:54" Update_time="2013-06-14 00:06:54" Collation="utf8_general_ci" Create_options="" Comment="Time zone transitions" />
	</table_structure>
	<table_data name="time_zone_transition">
	</table_data>
	<table_structure name="time_zone_transition_type">
		<field Field="Time_zone_id" Type="int(10) unsigned" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="Transition_type_id" Type="int(10) unsigned" Null="NO" Key="PRI" Extra="" Comment="" />
		<field Field="Offset" Type="int(11)" Null="NO" Key="" Default="0" Extra="" Comment="" />
		<field Field="Is_DST" Type="tinyint(3) unsigned" Null="NO" Key="" Default="0" Extra="" Comment="" />
		<field Field="Abbreviation" Type="char(8)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<key Table="time_zone_transition_type" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Time_zone_id" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="time_zone_transition_type" Non_unique="0" Key_name="PRIMARY" Seq_in_index="2" Column_name="Transition_type_id" Collation="A" Cardinality="0" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="time_zone_transition_type" Engine="MyISAM" Version="10" Row_format="Fixed" Rows="0" Avg_row_length="0" Data_length="0" Max_data_length="10696049115004927" Index_length="1024" Data_free="0" Create_time="2013-06-14 00:06:54" Update_time="2013-06-14 00:06:54" Collation="utf8_general_ci" Create_options="" Comment="Time zone transition types" />
	</table_structure>
	<table_data name="time_zone_transition_type">
	</table_data>
	<table_structure name="user">
		<field Field="Host" Type="char(60)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="User" Type="char(16)" Null="NO" Key="PRI" Default="" Extra="" Comment="" />
		<field Field="Password" Type="char(41)" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="Select_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Insert_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Update_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Delete_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Drop_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Reload_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Shutdown_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Process_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="File_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Grant_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="References_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Index_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Alter_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Show_db_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Super_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_tmp_table_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Lock_tables_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Execute_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Repl_slave_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Repl_client_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_view_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Show_view_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_routine_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Alter_routine_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_user_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Event_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Trigger_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="Create_tablespace_priv" Type="enum('N','Y')" Null="NO" Key="" Default="N" Extra="" Comment="" />
		<field Field="ssl_type" Type="enum('','ANY','X509','SPECIFIED')" Null="NO" Key="" Default="" Extra="" Comment="" />
		<field Field="ssl_cipher" Type="blob" Null="NO" Key="" Extra="" Comment="" />
		<field Field="x509_issuer" Type="blob" Null="NO" Key="" Extra="" Comment="" />
		<field Field="x509_subject" Type="blob" Null="NO" Key="" Extra="" Comment="" />
		<field Field="max_questions" Type="int(11) unsigned" Null="NO" Key="" Default="0" Extra="" Comment="" />
		<field Field="max_updates" Type="int(11) unsigned" Null="NO" Key="" Default="0" Extra="" Comment="" />
		<field Field="max_connections" Type="int(11) unsigned" Null="NO" Key="" Default="0" Extra="" Comment="" />
		<field Field="max_user_connections" Type="int(11) unsigned" Null="NO" Key="" Default="0" Extra="" Comment="" />
		<field Field="plugin" Type="char(64)" Null="YES" Key="" Default="" Extra="" Comment="" />
		<field Field="authentication_string" Type="text" Null="YES" Key="" Extra="" Comment="" />
		<key Table="user" Non_unique="0" Key_name="PRIMARY" Seq_in_index="1" Column_name="Host" Collation="A" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<key Table="user" Non_unique="0" Key_name="PRIMARY" Seq_in_index="2" Column_name="User" Collation="A" Cardinality="7" Null="" Index_type="BTREE" Comment="" Index_comment="" />
		<options Name="user" Engine="MyISAM" Version="10" Row_format="Dynamic" Rows="7" Avg_row_length="82" Data_length="580" Max_data_length="281474976710655" Index_length="2048" Data_free="0" Create_time="2013-06-14 00:07:20" Update_time="2013-06-14 01:26:32" Collation="utf8_bin" Create_options="" Comment="Users and global privileges" />
	</table_structure>
	<table_data name="user">
	<row>
		<field name="Host">localhost</field>
		<field name="User">root</field>
		<field name="Password">*D37C49F9CBEFBF8B6F4B165AC703AA271E079004</field>
		<field name="Select_priv">Y</field>
		<field name="Insert_priv">Y</field>
		<field name="Update_priv">Y</field>
		<field name="Delete_priv">Y</field>
		<field name="Create_priv">Y</field>
		<field name="Drop_priv">Y</field>
		<field name="Reload_priv">Y</field>
		<field name="Shutdown_priv">Y</field>
		<field name="Process_priv">Y</field>
		<field name="File_priv">Y</field>
		<field name="Grant_priv">Y</field>
		<field name="References_priv">Y</field>
		<field name="Index_priv">Y</field>
		<field name="Alter_priv">Y</field>
		<field name="Show_db_priv">Y</field>
		<field name="Super_priv">Y</field>
		<field name="Create_tmp_table_priv">Y</field>
		<field name="Lock_tables_priv">Y</field>
		<field name="Execute_priv">Y</field>
		<field name="Repl_slave_priv">Y</field>
		<field name="Repl_client_priv">Y</field>
		<field name="Create_view_priv">Y</field>
		<field name="Show_view_priv">Y</field>
		<field name="Create_routine_priv">Y</field>
		<field name="Alter_routine_priv">Y</field>
		<field name="Create_user_priv">Y</field>
		<field name="Event_priv">Y</field>
		<field name="Trigger_priv">Y</field>
		<field name="Create_tablespace_priv">Y</field>
		<field name="ssl_type"></field>
		<field name="ssl_cipher"></field>
		<field name="x509_issuer"></field>
		<field name="x509_subject"></field>
		<field name="max_questions">0</field>
		<field name="max_updates">0</field>
		<field name="max_connections">0</field>
		<field name="max_user_connections">0</field>
		<field name="plugin"></field>
		<field name="authentication_string"></field>
	</row>
	<row>
		<field name="Host">onedirac</field>
		<field name="User">root</field>
		<field name="Password">*D37C49F9CBEFBF8B6F4B165AC703AA271E079004</field>
		<field name="Select_priv">Y</field>
		<field name="Insert_priv">Y</field>
		<field name="Update_priv">Y</field>
		<field name="Delete_priv">Y</field>
		<field name="Create_priv">Y</field>
		<field name="Drop_priv">Y</field>
		<field name="Reload_priv">Y</field>
		<field name="Shutdown_priv">Y</field>
		<field name="Process_priv">Y</field>
		<field name="File_priv">Y</field>
		<field name="Grant_priv">Y</field>
		<field name="References_priv">Y</field>
		<field name="Index_priv">Y</field>
		<field name="Alter_priv">Y</field>
		<field name="Show_db_priv">Y</field>
		<field name="Super_priv">Y</field>
		<field name="Create_tmp_table_priv">Y</field>
		<field name="Lock_tables_priv">Y</field>
		<field name="Execute_priv">Y</field>
		<field name="Repl_slave_priv">Y</field>
		<field name="Repl_client_priv">Y</field>
		<field name="Create_view_priv">Y</field>
		<field name="Show_view_priv">Y</field>
		<field name="Create_routine_priv">Y</field>
		<field name="Alter_routine_priv">Y</field>
		<field name="Create_user_priv">Y</field>
		<field name="Event_priv">Y</field>
		<field name="Trigger_priv">Y</field>
		<field name="Create_tablespace_priv">Y</field>
		<field name="ssl_type"></field>
		<field name="ssl_cipher"></field>
		<field name="x509_issuer"></field>
		<field name="x509_subject"></field>
		<field name="max_questions">0</field>
		<field name="max_updates">0</field>
		<field name="max_connections">0</field>
		<field name="max_user_connections">0</field>
		<field name="plugin"></field>
		<field name="authentication_string"></field>
	</row>
	<row>
		<field name="Host">127.0.0.1</field>
		<field name="User">root</field>
		<field name="Password">*D37C49F9CBEFBF8B6F4B165AC703AA271E079004</field>
		<field name="Select_priv">Y</field>
		<field name="Insert_priv">Y</field>
		<field name="Update_priv">Y</field>
		<field name="Delete_priv">Y</field>
		<field name="Create_priv">Y</field>
		<field name="Drop_priv">Y</field>
		<field name="Reload_priv">Y</field>
		<field name="Shutdown_priv">Y</field>
		<field name="Process_priv">Y</field>
		<field name="File_priv">Y</field>
		<field name="Grant_priv">Y</field>
		<field name="References_priv">Y</field>
		<field name="Index_priv">Y</field>
		<field name="Alter_priv">Y</field>
		<field name="Show_db_priv">Y</field>
		<field name="Super_priv">Y</field>
		<field name="Create_tmp_table_priv">Y</field>
		<field name="Lock_tables_priv">Y</field>
		<field name="Execute_priv">Y</field>
		<field name="Repl_slave_priv">Y</field>
		<field name="Repl_client_priv">Y</field>
		<field name="Create_view_priv">Y</field>
		<field name="Show_view_priv">Y</field>
		<field name="Create_routine_priv">Y</field>
		<field name="Alter_routine_priv">Y</field>
		<field name="Create_user_priv">Y</field>
		<field name="Event_priv">Y</field>
		<field name="Trigger_priv">Y</field>
		<field name="Create_tablespace_priv">Y</field>
		<field name="ssl_type"></field>
		<field name="ssl_cipher"></field>
		<field name="x509_issuer"></field>
		<field name="x509_subject"></field>
		<field name="max_questions">0</field>
		<field name="max_updates">0</field>
		<field name="max_connections">0</field>
		<field name="max_user_connections">0</field>
		<field name="plugin"></field>
		<field name="authentication_string"></field>
	</row>
	<row>
		<field name="Host">::1</field>
		<field name="User">root</field>
		<field name="Password">*D37C49F9CBEFBF8B6F4B165AC703AA271E079004</field>
		<field name="Select_priv">Y</field>
		<field name="Insert_priv">Y</field>
		<field name="Update_priv">Y</field>
		<field name="Delete_priv">Y</field>
		<field name="Create_priv">Y</field>
		<field name="Drop_priv">Y</field>
		<field name="Reload_priv">Y</field>
		<field name="Shutdown_priv">Y</field>
		<field name="Process_priv">Y</field>
		<field name="File_priv">Y</field>
		<field name="Grant_priv">Y</field>
		<field name="References_priv">Y</field>
		<field name="Index_priv">Y</field>
		<field name="Alter_priv">Y</field>
		<field name="Show_db_priv">Y</field>
		<field name="Super_priv">Y</field>
		<field name="Create_tmp_table_priv">Y</field>
		<field name="Lock_tables_priv">Y</field>
		<field name="Execute_priv">Y</field>
		<field name="Repl_slave_priv">Y</field>
		<field name="Repl_client_priv">Y</field>
		<field name="Create_view_priv">Y</field>
		<field name="Show_view_priv">Y</field>
		<field name="Create_routine_priv">Y</field>
		<field name="Alter_routine_priv">Y</field>
		<field name="Create_user_priv">Y</field>
		<field name="Event_priv">Y</field>
		<field name="Trigger_priv">Y</field>
		<field name="Create_tablespace_priv">Y</field>
		<field name="ssl_type"></field>
		<field name="ssl_cipher"></field>
		<field name="x509_issuer"></field>
		<field name="x509_subject"></field>
		<field name="max_questions">0</field>
		<field name="max_updates">0</field>
		<field name="max_connections">0</field>
		<field name="max_user_connections">0</field>
		<field name="plugin"></field>
		<field name="authentication_string"></field>
	</row>
	<row>
		<field name="Host">localhost</field>
		<field name="User"></field>
		<field name="Password"></field>
		<field name="Select_priv">N</field>
		<field name="Insert_priv">N</field>
		<field name="Update_priv">N</field>
		<field name="Delete_priv">N</field>
		<field name="Create_priv">N</field>
		<field name="Drop_priv">N</field>
		<field name="Reload_priv">N</field>
		<field name="Shutdown_priv">N</field>
		<field name="Process_priv">N</field>
		<field name="File_priv">N</field>
		<field name="Grant_priv">N</field>
		<field name="References_priv">N</field>
		<field name="Index_priv">N</field>
		<field name="Alter_priv">N</field>
		<field name="Show_db_priv">N</field>
		<field name="Super_priv">N</field>
		<field name="Create_tmp_table_priv">N</field>
		<field name="Lock_tables_priv">N</field>
		<field name="Execute_priv">N</field>
		<field name="Repl_slave_priv">N</field>
		<field name="Repl_client_priv">N</field>
		<field name="Create_view_priv">N</field>
		<field name="Show_view_priv">N</field>
		<field name="Create_routine_priv">N</field>
		<field name="Alter_routine_priv">N</field>
		<field name="Create_user_priv">N</field>
		<field name="Event_priv">N</field>
		<field name="Trigger_priv">N</field>
		<field name="Create_tablespace_priv">N</field>
		<field name="ssl_type"></field>
		<field name="ssl_cipher"></field>
		<field name="x509_issuer"></field>
		<field name="x509_subject"></field>
		<field name="max_questions">0</field>
		<field name="max_updates">0</field>
		<field name="max_connections">0</field>
		<field name="max_user_connections">0</field>
		<field name="plugin"></field>
		<field name="authentication_string" xsi:nil="true" />
	</row>
	<row>
		<field name="Host">onedirac</field>
		<field name="User"></field>
		<field name="Password"></field>
		<field name="Select_priv">N</field>
		<field name="Insert_priv">N</field>
		<field name="Update_priv">N</field>
		<field name="Delete_priv">N</field>
		<field name="Create_priv">N</field>
		<field name="Drop_priv">N</field>
		<field name="Reload_priv">N</field>
		<field name="Shutdown_priv">N</field>
		<field name="Process_priv">N</field>
		<field name="File_priv">N</field>
		<field name="Grant_priv">N</field>
		<field name="References_priv">N</field>
		<field name="Index_priv">N</field>
		<field name="Alter_priv">N</field>
		<field name="Show_db_priv">N</field>
		<field name="Super_priv">N</field>
		<field name="Create_tmp_table_priv">N</field>
		<field name="Lock_tables_priv">N</field>
		<field name="Execute_priv">N</field>
		<field name="Repl_slave_priv">N</field>
		<field name="Repl_client_priv">N</field>
		<field name="Create_view_priv">N</field>
		<field name="Show_view_priv">N</field>
		<field name="Create_routine_priv">N</field>
		<field name="Alter_routine_priv">N</field>
		<field name="Create_user_priv">N</field>
		<field name="Event_priv">N</field>
		<field name="Trigger_priv">N</field>
		<field name="Create_tablespace_priv">N</field>
		<field name="ssl_type"></field>
		<field name="ssl_cipher"></field>
		<field name="x509_issuer"></field>
		<field name="x509_subject"></field>
		<field name="max_questions">0</field>
		<field name="max_updates">0</field>
		<field name="max_connections">0</field>
		<field name="max_user_connections">0</field>
		<field name="plugin"></field>
		<field name="authentication_string" xsi:nil="true" />
	</row>
	<row>
		<field name="Host">localhost</field>
		<field name="User">debian-sys-maint</field>
		<field name="Password">*78B26924994EC0CD9A298B6548F79259FE37C7C0</field>
		<field name="Select_priv">Y</field>
		<field name="Insert_priv">Y</field>
		<field name="Update_priv">Y</field>
		<field name="Delete_priv">Y</field>
		<field name="Create_priv">Y</field>
		<field name="Drop_priv">Y</field>
		<field name="Reload_priv">Y</field>
		<field name="Shutdown_priv">Y</field>
		<field name="Process_priv">Y</field>
		<field name="File_priv">Y</field>
		<field name="Grant_priv">Y</field>
		<field name="References_priv">Y</field>
		<field name="Index_priv">Y</field>
		<field name="Alter_priv">Y</field>
		<field name="Show_db_priv">Y</field>
		<field name="Super_priv">Y</field>
		<field name="Create_tmp_table_priv">Y</field>
		<field name="Lock_tables_priv">Y</field>
		<field name="Execute_priv">Y</field>
		<field name="Repl_slave_priv">Y</field>
		<field name="Repl_client_priv">Y</field>
		<field name="Create_view_priv">Y</field>
		<field name="Show_view_priv">Y</field>
		<field name="Create_routine_priv">Y</field>
		<field name="Alter_routine_priv">Y</field>
		<field name="Create_user_priv">Y</field>
		<field name="Event_priv">Y</field>
		<field name="Trigger_priv">Y</field>
		<field name="Create_tablespace_priv">Y</field>
		<field name="ssl_type"></field>
		<field name="ssl_cipher"></field>
		<field name="x509_issuer"></field>
		<field name="x509_subject"></field>
		<field name="max_questions">0</field>
		<field name="max_updates">0</field>
		<field name="max_connections">0</field>
		<field name="max_user_connections">0</field>
		<field name="plugin"></field>
		<field name="authentication_string" xsi:nil="true" />
	</row>
	</table_data>
</database>
<database name="newdb">
</database>
<database name="test">
</database>
</mysqldump>
