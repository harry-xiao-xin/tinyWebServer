#!/bin/sh
apt-get update --fix-missing -y
apt install -y mysql-server mysql-client libmysqlclient-dev
service mysql restart
service mysql status
wget https://tangentsoft.com/mysqlpp/releases/mysql++-3.2.5.tar.gz
tar -zxvf ./mysql++-3.2.5.tar.gz
cd  mysql++-3.2.5
 ./configure
make -j8
make install