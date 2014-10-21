git add Roteamento/; 
git commit -m"nao ha tempo para mensagens"; 
git push; 
cd Roteamento/src/; 
find . -name "*.class"|xargs rm -rf
javac Principal.java; 
java Principal   192.168.1.42 192.168.1.47;
cd ..; 
cd ..;
