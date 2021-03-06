/**
THIS MODULE IMPLEMENTS OPERATIONS REGARDING CLASS PREDICTION.
*/

#include "core.h"

int projection_size=1, count_target[MAX_CLASSES];
float avg_size=0;

static int test_cmp(const void* a, const void* b) {
	active_t* i=(active_t*)a;
	active_t* j=(active_t*)b;
	//printf("numero teste %d \n",atoi((*i).test.id)); 
	if((*i).processed>(*j).processed) return(1);
	if((*i).processed<(*j).processed) return(-1);
	if((*i).n_rules>(*j).n_rules) return(1);
	if((*i).n_rules<(*j).n_rules) return(-1);
	if (OCCURS_CMP==0) {
        if((*i).total_occurs>(*j).total_occurs) return(-1);
        if((*i).total_occurs<(*j).total_occurs) return(1);
    }
    else {
        if (OCCURS_CMP==1) {
             if (TRAIN_EMPTY==1) {
                if((*i).total_occurs>(*j).total_occurs) return(-1);
                if((*i).total_occurs<(*j).total_occurs) return(1);
             } else {
                if((*i).total_occurs>(*j).total_occurs) return(1);
                if((*i).total_occurs<(*j).total_occurs) return(-1);
             }
        }
        else {
            if (TRAIN_EMPTY==1) {
                if((*i).total_occurs>(*j).total_occurs) return(1);
                if((*i).total_occurs<(*j).total_occurs) return(-1);
             } else {
                if((*i).total_occurs>(*j).total_occurs) return(-1);
                if((*i).total_occurs<(*j).total_occurs) return(1);
             }
        }
    }
	if((*i).diff/(float)((*i).n_rules+1)>(*j).diff/(float)((*j).n_rules+1)) return(1);
	if((*i).diff/(float)((*i).n_rules+1)<(*j).diff/(float)((*j).n_rules+1)) return(-1);
	if(atoi((*i).test.id)>atoi((*j).test.id)) return (-1);
	if(atoi((*i).test.id)<atoi((*j).test.id)) return (1);
	return(0);
}

static int item_cmp(const void* a, const void* b) {
	int* i=(int*)a;
	int* j=(int*)b;
//do menor para o maior
	if(ITEMSETS[*i].count>ITEMSETS[*j].count) return(1);
	if(ITEMSETS[*i].count<ITEMSETS[*j].count) return(-1);
	return(0);
}

/**
PRINTS ALL STATISTICS ASSOCIATED WITH A PREDICTION.
*/
void print_statistics(int n_tests, int true_label, char* id, prediction_t prediction, evaluation_t evaluator) {
	float ranking=0, entropy=0;
	for(int i=0;i<MAX_CLASSES;i++) ranking+=i*prediction.score.points[i];
	for(int i=0;i<MAX_CLASSES;i++) entropy-=prediction.score.points[i]*log2(prediction.score.points[i]);
	float* rel=(float*)malloc(sizeof(float)*MAX_CLASSES);
	for(int i=0;i<MAX_CLASSES;i++) rel[i]=count_target[i]/(float)projection_size;
	printf("%d-%ld %f%% id= %s label= %d correct= %d", n_tests, N_TESTS, 100*n_tests/(float)N_TESTS, id, true_label, prediction.correct);
	printf(" prediction= %d", prediction.label);
	printf(" ranking= %f entropy= %f rules= %d projection= %f avg_size= %f ", ranking, entropy, N_RULES, projection_size/(float)N_TRANSACTIONS, avg_size);
	for(int i=0;i<MAX_CLASSES;i++) {
		printf("Rel[%d]= %f Score[%d]= %f ", i, rel[i], i, prediction.score.points[i]);
		printf("Prec[%d]= %f Rec[%d]= %f F1[%d]= %f    ", i, evaluator.precision[i], i, evaluator.recall[i], i, evaluator.f1[i]);
	}
	printf("MF1= %f Acc= %f ", evaluator.mf1, evaluator.acc);
	printf("Cache= %f\n", CACHE.content.size()/(float)CACHE.max_size);
	free(rel);
}

/**
INDUCES A RULE SET. THE RULE SET KEEPS GROWING UNTIL A CLASS IS SUFFICIENTLY BETTER THAN ANOTHER.
THEN, PERFORMS THE PREDICTION.
*/
prediction_t get_THE_prediction(int* instance, int instance_size, int true_label) {
	
	prediction_t prediction;
	int n_rules[2]={0,0}, min_count=MIN_COUNT, init=1, size=MIN_SIZE, n_items=0, default_prediction=0;
	int* tids=(int*)malloc(sizeof(int)*N_TRANSACTIONS);
	int* items=(int*) malloc(sizeof(int)*instance_size);
	
	for(int i=0;i<instance_size;i++) {
		if(instance[i]<N_ITEMSETS) items[n_items++]=instance[i];
		  
	}
	qsort((int*) items, n_items, sizeof(int), item_cmp);
	reset_rules();
	while(size<MAX_SIZE) {
		if(init) {
			init=0;
			projection_size=project_training(items, n_items, tids);
			for(int i=0;i<MAX_CLASSES;i++) count_target[i]=0;
			//for(int i=0;i<projection_size;i++) count_target[TID_CLASSES[tids[i]]]++;
			for(int i=1;i<MAX_CLASSES;i++) {
				if(count_target[i]>=count_target[default_prediction]) default_prediction=i;
			}
		}
		if(RELATIVE) min_count=(int)(MIN_SUPP*projection_size);
		induce_rules(items, n_items, count_target, size, projection_size, min_count);
		n_rules[0]=n_rules[1];
		n_rules[1]=N_RULES;
		size++;
		if(size==MAX_SIZE || size>n_items || n_rules[0]==n_rules[1]) break;
	}
	prediction.score=get_TOTAL_score();
	prediction.label=prediction.score.ordered_labels[0];
	free(tids);
	free(items);
	if(N_RULES==0) prediction.label=default_prediction;

	avg_size=0;
	for(int i=0;i<N_RULES;i++) avg_size+=RULES[i].size;
	if(N_RULES>0) avg_size/=N_RULES;
	prediction.correct=(prediction.label==true_label);
	return(prediction);
}

int lazy_supervised_classification() {
	__START_TIMER__
	CACHE.locked=0;
	evaluation_t evaluator;
	prediction_t prediction;
	initialize_evaluation(&evaluator);
	int n_tests=1;
	list_t* test=TEST;
	while(test) {
		struct timeval s; gettimeofday(&s, NULL);
		prediction=get_THE_prediction(test->instance, test->size, test->label);
		struct timeval e; gettimeofday(&e, NULL);
		long long tt=(e.tv_sec-s.tv_sec)*1000000+e.tv_usec-s.tv_usec;
		printf("time: %d %lf\n", n_tests, tt/(double)1000000);
		update_evaluation(&evaluator, prediction.label, test->label);
		print_statistics(n_tests, test->label, test->id, prediction, evaluator);
		n_tests++;
		list_t* x=test->next;
		free(test->instance);
		free(test);
		test=x;
	}
	printf("\n");
	for(int i=0;i<MAX_CLASSES;i++) printf("CLASS(%d)= %d Prec= %f Rec= %f F1= %f   ", i, evaluator.true_labels[i], evaluator.precision[i], evaluator.recall[i], evaluator.f1[i]);
	printf("Acc= %f  MF1= %f   *    hits= %ld misses= %ld\n", evaluator.acc, evaluator.mf1, CACHE.hits, CACHE.misses);
	finalize_evaluation(&evaluator);
	__FINISH_TIMER__
	return(0);
}

int lazy_active_classification() {
	__START_TIMER__
	CACHE.locked=0;
	int judgements=0, n_tests=0;
	map<int, int> occurs;
	list_t* test=TEST;
	//active_t ordered_tests[N_TESTS];
	active_t* ordered_tests=(active_t*)malloc(sizeof(active_t)*N_TESTS);
	while(test) {
		ordered_tests[n_tests].processed=0;
		ordered_tests[n_tests].diff=0;
		ordered_tests[n_tests].total_occurs=0;
		ordered_tests[n_tests].n_rules=0;
		ordered_tests[n_tests].test.id=strdup(test->id);
		ordered_tests[n_tests].test.label=test->label;
		ordered_tests[n_tests].test.size=test->size;
		ordered_tests[n_tests].test.instance=(int*)malloc(sizeof(int)*ordered_tests[n_tests].test.size);
		for(int i=0;i<test->size;i++) {
			ordered_tests[n_tests].test.instance[i]=test->instance[i];
			if(occurs.find(test->instance[i])==occurs.end()) occurs[test->instance[i]]=0;
			occurs[test->instance[i]]++;
		}
		n_tests++;
		list_t* x=test->next;
		test=x;
	}
	//printf("numero teste %d",n_tests); 
	while(judgements<MAX_JUDGEMENTS) {
		//printf("status[%d-%d]:           ", judgements, MAX_JUDGEMENTS);
		for(int i=0;i<n_tests;i++) {
			if(ordered_tests[i].processed==0) {
				get_THE_prediction(ordered_tests[i].test.instance, ordered_tests[i].test.size, ordered_tests[i].test.label);
				ordered_tests[i].diff=N_RULES-ordered_tests[i].n_rules;
				ordered_tests[i].n_rules=N_RULES;
				ordered_tests[i].total_occurs=0;
				for(int j=0;j<ordered_tests[i].test.size;j++) 
				  ordered_tests[i].total_occurs+=occurs[ordered_tests[i].test.instance[j]];
			}
			char done[100];
			for(unsigned int j=0;j<strlen(done);j++) printf("\b");
			sprintf(done,"%d-%d  %d%%",CACHE.content.size(), CACHE.max_size, (int)(100*i/n_tests));
			//printf("%s", done);
			fflush(stdout);
		}
		printf("\n");
		CACHE.hits=CACHE.misses=0;
		CACHE.content.clear();
		qsort((active_t*) ordered_tests, n_tests, sizeof(active_t), test_cmp);
		ordered_tests[0].processed=1;
		printf("%s total_occurs %d n_rules %d diff %d \n", ordered_tests[0].test.id, ordered_tests[0].total_occurs, ordered_tests[0].n_rules, ordered_tests[0].diff);
		printf("%s total_occurs %d n_rules %d diff %d \n", ordered_tests[1].test.id, ordered_tests[1].total_occurs, ordered_tests[1].n_rules, ordered_tests[1].diff);
		printf("TRAIN_EMPTY=%d \n", TRAIN_EMPTY);
		printf("OCCURS_CMP=%d \n", OCCURS_CMP);
		//for(int i=0;i<n_tests;i++) {
		//  printf("\n %i--------------------------- %s size %i total_occurs %d n_rules %d diff %d \n", i, ordered_tests[i].test.id,n_tests, ordered_tests[i].total_occurs, ordered_tests[i].n_rules, ordered_tests[i].diff);
		//}
		for(int i=0;i<ordered_tests[0].test.size;i++) occurs[ordered_tests[0].test.instance[i]]=0;

        //
		// check for repeated test instances and decide for a class (command line option -r 1)
		//
		if (CHECK_REPEATED==1) {
            int repclass[MAX_CLASSES];                         // class counter
            for(int i=0; i<MAX_CLASSES; i++) repclass[i]=0;    // reset counter
            repclass[ordered_tests[0].test.label]++;           // selected instance vote
            int repinstances=0;

            // loops through all instances in ordered_tests
            for(int i=1;i<n_tests;i++) {
                int repeat=1;
                // for each instance
                for(int j=0; j<ordered_tests[i].test.size;j++) {
                    if (ordered_tests[0].test.instance[j] != ordered_tests[i].test.instance[j]) {
                        // at least one feature value is different. This is not a repeated instance
                        repeat=0;
                        break;
                        }
                }
                if (repeat==1) {
                    // found a repeated instance (all features have the same value as the selected instance)
                    repinstances=1;
                   // printf("repeated instance %s %d\n", ordered_tests[i].test.id, ordered_tests[i].test.label);
                    repclass[ordered_tests[i].test.label]++; // casts vote for the repeated instance's class
                }
            }
            if (repinstances==1) {
                // at least one repeated instance found
                int votedclass=ordered_tests[0].test.label;
                int maxvotes=0;
                for(int i=0;i<MAX_CLASSES;i++) {
                    // which class has the most votes?
                    if(repclass[i]>maxvotes) {
                        maxvotes=repclass[i];
                        votedclass=i;
                      //  printf("Class %d has %d votes \n", i, repclass[i]);
                    }
                }
                if (votedclass!=ordered_tests[0].test.label) {
                    // A different class is more common amongst the repeated instances. Print this information.
                    printf("New CLASS = %d for instance %s (%d votes out of %d repeated instances)\n", votedclass,ordered_tests[0].test.id,maxvotes,repinstances);
                    ordered_tests[0].test.label=votedclass;
                }
            }
		}

        printf("inserting instance %s %d\n", ordered_tests[0].test.id, ordered_tests[0].test.label);
        fflush(stdout);

		N_TRANSACTIONS++;
		COUNT_TARGET[ordered_tests[0].test.label]++;
		//TID_CLASSES[N_TRANSACTIONS]=ordered_tests[0].test.label;
		if(COUNT_TARGET[ordered_tests[0].test.label]==1) {
			ITEMSETS[TARGET_ID[ordered_tests[0].test.label]].count=1;
			ITEMSETS[TARGET_ID[ordered_tests[0].test.label]].list=(int*)malloc(sizeof(int));
			ITEMSETS[TARGET_ID[ordered_tests[0].test.label]].list[0]=N_TRANSACTIONS;
		}
		else {
			ITEMSETS[TARGET_ID[ordered_tests[0].test.label]].list=(int*)realloc(ITEMSETS[TARGET_ID[ordered_tests[0].test.label]].list, sizeof(int)*(ITEMSETS[TARGET_ID[ordered_tests[0].test.label]].count+1));
			ITEMSETS[TARGET_ID[ordered_tests[0].test.label]].list[ITEMSETS[TARGET_ID[ordered_tests[0].test.label]].count]=N_TRANSACTIONS;
			ITEMSETS[TARGET_ID[ordered_tests[0].test.label]].count++;
		}
		for(int j=0;j<ordered_tests[0].test.size;j++) {
			if(ITEMSETS[ordered_tests[0].test.instance[j]].count==0) ITEMSETS[ordered_tests[0].test.instance[j]].list=(int*)malloc(sizeof(int));
			else ITEMSETS[ordered_tests[0].test.instance[j]].list=(int*)realloc(ITEMSETS[ordered_tests[0].test.instance[j]].list, sizeof(int)*(ITEMSETS[ordered_tests[0].test.instance[j]].count+1));
			ITEMSETS[ordered_tests[0].test.instance[j]].list[ITEMSETS[ordered_tests[0].test.instance[j]].count]=N_TRANSACTIONS;
			ITEMSETS[ordered_tests[0].test.instance[j]].count++;
		}
		judgements++;
	}
	for(int i=0;i<n_tests;i++) {
		free(ordered_tests[i].test.id);
		free(ordered_tests[i].test.instance);
	}
	//lazy_supervised_classification();
	
	print_rules();
	__FINISH_TIMER__
	return(0);
}

int lazy_transductive_classification() {
	__START_TIMER__
	CACHE.locked=1;
	evaluation_t evaluator;
	prediction_t prediction;
	initialize_evaluation(&evaluator);
	int abstain=0, n_tests=1, remaining_tests=N_TESTS, locked=1;
	float min_level=MIN_LEVEL;
	list_t* test=TEST;
	while(test) {
		reset_rules();
		prediction=get_THE_prediction(test->instance, test->size, test->label);
		//if(!locked || prediction.score.points[prediction.score.ordered_labels[0]]>min_level || prediction.score.points[prediction.score.ordered_labels[0]]/(float)prediction.score.points[prediction.score.ordered_labels[1]]>=FACTOR) {
		if(!locked || min_level<0.1 || (prediction.score.points[prediction.score.ordered_labels[0]]>min_level && prediction.score.points[prediction.score.ordered_labels[0]]<1)) {
			update_evaluation(&evaluator, prediction.label, test->label);
			//print_statistics(n_tests, test->label, test->id, prediction, evaluator);
			free(prediction.score.ordered_labels);
			free(prediction.score.points);
			if(locked) {
				N_TRANSACTIONS++;
				COUNT_TARGET[prediction.label]++;
				//TID_CLASSES[N_TRANSACTIONS]=prediction.label;
				if(COUNT_TARGET[prediction.label]==1) {
					ITEMSETS[TARGET_ID[prediction.label]].count=1;
					ITEMSETS[prediction.label].list[0]=N_TRANSACTIONS;
				}
				else {
					ITEMSETS[TARGET_ID[prediction.label]].list[ITEMSETS[TARGET_ID[prediction.label]].count]=N_TRANSACTIONS;
					ITEMSETS[TARGET_ID[prediction.label]].count++;
				}
				for(int j=0;j<test->size;j++) {
					ITEMSETS[test->instance[j]].list[ITEMSETS[test->instance[j]].count]=N_TRANSACTIONS;
					ITEMSETS[test->instance[j]].count++;
				}
			}
			n_tests++;
			remaining_tests--;
			abstain=0;
			list_t* x=test->next;
			free(test->instance);
			free(test);
			test=x;
			TEST=test;
		}
		else if(abstain<remaining_tests) {
printf("abstaining %d %d %f %f %f\n", abstain, remaining_tests, min_level, prediction.score.points[prediction.score.ordered_labels[0]]/(float)prediction.score.points[prediction.score.ordered_labels[1]], avg_size);
			abstain++;
        		list_t* q=TEST;
			while(q->next!=NULL) q=q->next;
			list_t* t=(list_t*)malloc(sizeof(list_t));
			t->instance=(int*)malloc(sizeof(int)*test->size);
			t->size=0;
			for(int i=0;i<test->size;i++) t->instance[t->size++]=test->instance[i];
			t->id=test->id;
			t->label=test->label;
			t->next=NULL;
			q->next=t;
			list_t* x=test->next;
			free(test->instance);
			free(test);
			test=x;
			TEST=test;
		}
		else locked=0;
	}
	//free(TID_CLASSES);
	printf("\n");
	for(int i=0;i<MAX_CLASSES;i++) printf("CLASS(%d)= %d Prec= %f Rec= %f F1= %f   ", i, evaluator.true_labels[i], evaluator.precision[i], evaluator.recall[i], evaluator.f1[i]);
	printf("Acc= %f  MF1= %f   *    hits= %ld misses= %ld\n", evaluator.acc, evaluator.mf1, CACHE.hits, CACHE.misses);
	finalize_evaluation(&evaluator);
	__FINISH_TIMER__
	return(0);
}

int lazy_semisupervised_classification() {
	__START_TIMER__
	CACHE.locked=1;
	evaluation_t evaluator;
	prediction_t prediction;
	initialize_evaluation(&evaluator);
	int abstain=0, n_points=1, remaining_points=N_POINTS;
	float min_level=MIN_LEVEL;
	list_t* unlabeled=UNLABELED;
	while(unlabeled) {
		reset_rules();
		prediction=get_THE_prediction(unlabeled->instance, unlabeled->size, unlabeled->label);
		if(prediction.score.points[prediction.score.ordered_labels[0]]>min_level || prediction.score.points[prediction.score.ordered_labels[0]]/(float)prediction.score.points[prediction.score.ordered_labels[1]]>=FACTOR) {
			update_evaluation(&evaluator, prediction.label, unlabeled->label);
			print_statistics(n_points, unlabeled->label, unlabeled->id, prediction, evaluator);
			N_TRANSACTIONS++;
			COUNT_TARGET[prediction.label]++;
			//TID_CLASSES[N_TRANSACTIONS]=prediction.label;
			ITEMSETS[TARGET_ID[prediction.label]].list[ITEMSETS[TARGET_ID[prediction.label]].count]=N_TRANSACTIONS;
			ITEMSETS[TARGET_ID[prediction.label]].count++;
			for(int j=0;j<unlabeled->size;j++) {
				ITEMSETS[unlabeled->instance[j]].list[ITEMSETS[unlabeled->instance[j]].count]=N_TRANSACTIONS;
				ITEMSETS[unlabeled->instance[j]].count++;
			}

			n_points++;
			remaining_points--;
			abstain=0;
			list_t* x=unlabeled->next;
			free(unlabeled->instance);
			free(unlabeled);
			unlabeled=x;
			UNLABELED=unlabeled;
		}
		else if(abstain<remaining_points) {
printf("abstaining %d %d %f %f %f\n", abstain, remaining_points, min_level, prediction.score.points[prediction.score.ordered_labels[0]]/(float)prediction.score.points[prediction.score.ordered_labels[1]], avg_size);
			abstain++;
        		list_t* q=UNLABELED;
			while(q->next!=NULL) q=q->next;
			list_t* t=(list_t*)malloc(sizeof(list_t));
			t->instance=(int*)malloc(sizeof(int)*unlabeled->size);
			t->size=0;
			for(int i=0;i<unlabeled->size;i++) t->instance[t->size++]=unlabeled->instance[i];
			t->label=unlabeled->label;
			t->next=NULL;
			q->next=t;
			list_t* x=unlabeled->next;
			free(unlabeled->instance);
			free(unlabeled);
			unlabeled=x;
			UNLABELED=unlabeled;
		}
		else break;
	}
	printf("\n");
	for(int i=0;i<MAX_CLASSES;i++) printf("CLASS(%d)= %d Prec= %f Rec= %f F1= %f   ", i, evaluator.true_labels[i], evaluator.precision[i], evaluator.recall[i], evaluator.f1[i]);
	printf("Acc= %f  MF1= %f   *    hits= %ld misses= %ld\n", evaluator.acc, evaluator.mf1, CACHE.hits, CACHE.misses);
	finalize_evaluation(&evaluator);
	__FINISH_TIMER__
	return(0);
}
