#!/bin/sh
wget https://tangentsoft.com/mysqlpp/releases/mysql++-3.2.5.tar.gz
tar -zxvf ./mysql++-3.2.5.tar.gz
cd  mysql++-3.2.5
 ./configure
make
make install