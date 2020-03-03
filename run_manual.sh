 #!/bin/bash
rules=5
# # 
suffix="SVM_testeRankingCorrelation"
#rm saida_Hall_L2R saida_wahono_L2R saida_kitchenham_L2R saida_Rad_L2R
#rm /tmp/saida_L2R_*
rm saida_*_$suffix
rm /tmp/saidaRank.*
( for i in `seq 0 3 `; do    
   
        #./run actransvm_SCAL_athome Hall 20 logistic_short Hall.svm.fil teste saida.Hall Hall $rules    &>> saida_hall_teste_slop        
	#./run actransvm_SCAL_athome_L2R Hall Hall.svm.fil teste saida.Hall 5 20 &>> saida_Hall_$suffix
	#./run actransvm_SCAL_athome_icde Hall Hall.svm.fil teste saida.Hall 5 20 &>> saida_Hall_$suffix
	./run actransvm_SCAL_athome_icde Hall Hall.svm.fil teste saida.Rad 5 20 &>> saida_Hall_$suffix 
 wait
done ) &

 sleep 3
 ( for i in `seq 0 3 ` ; do
    
 #    ./run actransvm_SCAL_athome_L2R wahono wahono.svm.fil teste saida.wahono 5 20 &>> saida_wahono_$suffix 
#         ./run actransvm_SCAL_athome wahono 20 logistic_short wahono.svm.fil teste saida.wahono wahono  $rules &>> saida_wahono_teste_slop
#./run actransvm_SCAL_athome_icde wahono 20 logistic_short wahono.svm.fil teste saida.wahono wahono  $rules &>> saida_wahono_$suffix
    ./run actransvm_SCAL_athome_icde wahono wahono.svm.fil teste saida.wahono 5 20 &>> saida_wahono_$suffix
#    
   # ./run actransvm_SCAL_athome_icde wahono wahono.svm.fil teste saida.wahono  &>> saida_wahono_icdev31 
  wait
 done ) &
#  sleep 3

 (for i in `seq 0 3 ` ; do
#        ./run actransvm_SCAL_athome_L2R kitchenham kitchenham.svm.fil teste saida.kitchenham 5 20 &>> saida_kitchenham_$suffix
#        ./run actransvm_SCAL_athome kitchenham 20 logistic_short kitchenham.svm.fil teste saida.kitchenham kitchenham $rules &>> saida_kitchenham_teste_slop
#./run actransvm_SCAL_athome_icde kitchenham 20 logistic_short kitchenham.svm.fil teste saida.kitchenham kitchenham $rules &>> saida_kitchenham_$suffix
   ./run actransvm_SCAL_athome_icde kitchenham kitchenham.svm.fil teste saida.kitchenham 5 20 &>> saida_kitchenham_$suffix
    wait
done) &

# sleep 3
(for i in `seq 0 3 ` ; do
     #./run actransvm_SCAL_athome_L2R Rad Rad.svm.fil teste saida.Rad 5 20 &>> saida_Rad_$suffix
     #  ./run actransvm_SCAL_athome Rad 20 logistic_short Rad.svm.fil teste saida.Rad Rad $rules &>>  saida_rad_teste_slop
     ./run actransvm_SCAL_athome_icde Rad Rad.svm.fil teste saida.Rad 5 20 &>> saida_Rad_$suffix
     #./run actransvm_SCAL_athome_icde Rad 20 logistic_short Rad.svm.fil teste saida.Rad Rad $rules &>>  saida_Rad_$suffix
   
   wait
done)

# 
# 
#sligind widnwos 
#!/bin/bash



#  rules=5
# 
# 
# (for j in 20 25 10 35 40; do   
#    for i in  `seq 0 30 `; do   
# 
#         ./run actransvm_SCAL_athome Hall $j logistic_short Hall.svm.fil teste saida.Hall Hall $rules  &>> saida_sliding_hall_v0       
#     done
#  wait
# done ) &
# 
# 
# (for j in 20 25 10 35 40; do   
#    for i in  `seq 0 30 `; do 
#         ./run actransvm_SCAL_athome wahono $j logistic_short wahono.svm.fil teste saida.wahono wahono $rules &>> saida_sliding_wahono_v0 
#     done
#  wait
# done ) &
# 
# 
#  (for j in 20 ; do   
#     for i in  `seq `; do 
#          ./run actransvm_SCAL_athome kitchenham $j logistic_short kitchenham.svm.fil teste saida.kitchenham kitchenham 10 &>> teste_rever
#      done
#      wait
#  done)
# 
# (for j in 20 25 10 35 40; do   
#    for i in  `seq 0 30 `; do 
#         ./run actransvm_SCAL_athome Rad $j logistic_short Rad.svm.fil teste saida.Rad Rad $rules &>>  saida_sliding_Rad_v0
#      done
#     wait
# done)
# # # 



#(for j in 1 10 entropy 20 25 10 35 40; do   
#   for i in  `seq 0 30`; do   #

#        ./run actransvm_SCAL_athome Hall $j logistic_short Hall.svm.fil teste saida.Hall Hall $rules  &>> saida_sliding_athome1_v0       
#    done
# wait
#done ) &


# (for j in `seq 0 30`; do   
#   for i in  `seq 10 30 `; do 
#        ./run actransvm_SCAL_athome wahono 10 logistic_short wahono.svm.fil teste saida.wahono wahono $rules &>> saida_wahono_recallat_knee     
#    done
# wait
# done )



# ./run actransvm_SCAL_athome Hall 20 logistic_short Hall.svm.fil teste saida.Hall Hall 10  &>> saida_hall_tempo &
# 
# ./run actransvm_SCAL_athome wahono 20 logistic_short wahono.svm.fil teste saida.wahono wahono 10  &>> saida_wahono_tempo      &
# 
# ./run actransvm_SCAL_athome Rad 20 logistic_short Rad.svm.fil teste saida.Rad Rad 10 &>>  saida_Rad_tempo &
# 
# ./run actransvm_SCAL_athome kitchenham 20 logistic_short kitchenham.svm.fil teste saida.kitchenham kitchenham 10 &>> saida_kitchenham_tempo

