cd Roteamento/src/;
find . -name "*.class"|xargs rm -rf;
javac Principal.java; 
java Principal   192.168.1.39 192.168.1.47 ;
cd ..; 
cd ..;