cd graphs

shopt -s globstar nullglob; 
for file in **/*; do 
     dot -Tpng:cairo:gd $file -o $file.png 2> /dev/null;
done 
