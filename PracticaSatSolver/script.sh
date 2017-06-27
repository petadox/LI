#! /bin/bash
# -*- ENCODING: UTF-8 -*-

g++ -c -O3 satsolver2.cpp
g++ -o satsolver2 satsolver2.o

clausulas=100
var1=350
var2=10
rm Misresultados.txt
while [ $clausulas != $var1 ]
do
   echo "vars-"$clausulas"-"$var2".cnf" >> Misresultados.txt
   echo "=========================" >> Misresultados.txt
   ./satsolver2 < vars-$clausulas-$var2.cnf >> Misresultados.txt 2>> Misresultados.txt
   echo $'\n' >> Misresultados.txt
   juegodepruebas=1
   while [ $juegodepruebas != $var2 ]
   do
      echo "vars-"$clausulas"-"$juegodepruebas".cnf" >> Misresultados.txt
      echo "=========================" >> Misresultados.txt
      ./satsolver2 < vars-$clausulas-$juegodepruebas.cnf >> Misresultados.txt 2>> Misresultados.txt
      echo $'\n' >> Misresultados.txt
      ((juegodepruebas+=1))
   done
   ((clausulas+=50))
done
