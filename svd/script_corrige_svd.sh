
input=$1
cp ../logistic_short/$input /tmp/athome1
#for i in `seq 0 9`; do

#  cat ../logistic_short/athome10`echo $i`.synthetic.seed | sed 's/^1/athome10'`echo $i`'/' >> /tmp/athome
  
#done

cut -d' ' -f2- /tmp/athome1 > /tmp/athome1_2
cut -d' ' -f1 /tmp/athome1 > /tmp/labels
sed -i 's/^/0 /' /tmp/athome1_2

python3 trans.py /tmp/athome1_2  out $2

sed -i 's/^0//' out
paste /tmp/labels out -d ' ' | sed 's/  / /'  > ../logistic_short/$input.svd_$2
