#!/usr/bin/bash

source ./config.sh

INC_FILENAME=testerconf.h

if [  -f $INC_FILENAME ]; then
  rm $INC_FILENAME
fi

echo " " > $INC_FILENAME
echo "// DO NOT EDIT: this is an auto generated config file for testing swatdb" >> $INC_FILENAME
echo "//              (created by ./mktestconf.sh) " >> $INC_FILENAME
echo " " >> $INC_FILENAME
echo "static std::string testdb_file = \"$MYDBDIR/tables.db\";" >> $INC_FILENAME
echo "static std::string testdb_save_file = \"$MYDBDIR/tables_save.db\";" >> $INC_FILENAME
echo "static std::string testdb_dir = \"$MYDBDIR/\";" >> $INC_FILENAME

echo " " >> $INC_FILENAME
echo "static std::string smalldb_file = \"$MYDBDIR/small.db\";" >> $INC_FILENAME
echo "static std::string smalldb_save_file = \"$MYDBDIR/small_save.db\";" >> $INC_FILENAME



