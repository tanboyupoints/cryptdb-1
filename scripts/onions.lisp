
(DEFUN ADJUST-MAIN ()
  "onion adjuster for database: burrows+one, created: 12/8/2013 3:30:10"
  (LET* ((DB "dbnew")
         (CONN
          (CLSQL-SYS:CONNECT (LIST "127.0.0.1" DB "root" "letmein" 3307)
                             :DATABASE-TYPE :MYSQL :IF-EXISTS :NEW
                             :MAKE-DEFAULT NIL)))
    (UNWIND-PROTECT
        (PROGN
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_commentmeta" "comment_id" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_commentmeta" "meta_id" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_commentmeta" "meta_key" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_commentmeta" "meta_value" "oOPE" "OPE"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_comments" "comment_ID" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_comments" "comment_approved" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_comments" "comment_author" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_comments" "comment_author_email" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_comments" "comment_author_url" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_comments" "comment_content" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_comments" "comment_date" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_comments" "comment_date_gmt" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_comments" "comment_parent" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_comments" "comment_post_ID" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_comments" "comment_post_ID" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_comments" "user_id" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_links" "link_id" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_links" "link_updated" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_options" "autoload" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_options" "option_id" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_options" "option_name" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_options" "option_name" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_postmeta" "meta_id" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_postmeta" "meta_key" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_postmeta" "meta_key" "oOPE" "OPE"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_postmeta" "meta_key" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_postmeta" "meta_value" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_postmeta" "meta_value" "oOPE" "OPE"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_postmeta" "post_id" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_postmeta" "post_id" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "ID" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "menu_order" "oOPE" "OPE"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_author" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_date" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_date_gmt" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_mime_type" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_modified" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_modified_gmt" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_name" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_name" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_parent" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_parent" "oOPE" "OPE"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_status" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_title" "oOPE" "OPE"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "post_type" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_posts" "to_ping" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_term_relationships" "object_id" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_term_relationships" "object_id" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_term_relationships" "term_taxonomy_id" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_term_relationships" "term_taxonomy_id" "oOPE" "OPE"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB
                 '("wp_term_relationships" "term_taxonomy_id" "oPLAIN"
                   "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_term_taxonomy" "count" "oOPE" "OPE"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_term_taxonomy" "taxonomy" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_term_taxonomy" "term_id" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_term_taxonomy" "term_id" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB
                 '("wp_term_taxonomy" "term_taxonomy_id" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_terms" "name" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_terms" "name" "oOPE" "OPE"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_terms" "slug" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_terms" "term_id" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_usermeta" "meta_key" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_usermeta" "meta_value" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_usermeta" "umeta_id" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_usermeta" "user_id" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_usermeta" "user_id" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_users" "ID" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_users" "display_name" "oOPE" "OPE"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_users" "user_email" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_users" "user_login" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_users" "user_login" "oOPE" "OPE"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_users" "user_nicename" "oDET" "DET"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_users" "user_registered" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN)
         (CLSQL-SYS:QUERY
          (APPLY #'FORMAT NIL "SET @cryptdb='adjust',
                                         @database='~A',
                                         @table='~A',
                                         @field='~A',
                                         @~A='~A'"
                 DB '("wp_users" "user_url" "oPLAIN" "PLAINVAL"))
          :DATABASE CONN))
      (CLSQL-SYS:DISCONNECT :DATABASE CONN)))
  NIL)
