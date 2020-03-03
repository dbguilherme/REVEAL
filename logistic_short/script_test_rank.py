import sys
from scipy import stats
from scipy.stats import rankdata

print("file input is ", sys.argv[1], sys.argv[2])
from numpy import loadtxt
vetA = loadtxt(sys.argv[1],  delimiter="\n", unpack=False,dtype =float)
vetB = loadtxt(sys.argv[2],  delimiter="\n", unpack=False,dtype =float)

rankA=rankdata(vetA.tolist())
rankB=rankdata(vetB.tolist())

print("vector size is", len (vetA), len (vetB))
print("correlation spearman", stats.spearmanr(rankA, rankB), " ",sys.argv[3])
print("correlation kendall", stats.kendalltau(rankA, rankB), " ",sys.argv[3])
print("correlation weightedtau", stats.weightedtau(rankA, rankB), " ",sys.argv[3])

