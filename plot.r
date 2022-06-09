
data=read.csv('test.csv')
data1=read.csv('test1.csv')
data2=read.csv('test2.csv')
options(scipen = 999)
print(data)
pdf("jaccard4.pdf")
plot(0, 0, 
     xlab = "K",
     ylab = "Jaccard coefficient",
     main = "Jaccard score",
     type = "l",
     xlim = c(1,325729),
     ylim = c(0,1)
)
lines(data$k, y = data$value, type = "l", col = "blue")
lines(data$k, y = data1$value, type = "l", col = "red")
lines(data$k, y = data2$value, type = "l", col = "darkgreen")
par(xpd=TRUE)
legend(x = "topright", legend=c("PageRank - HITS", "PageRank - InDegree", "HITS authority - InDegree"), col=c("blue", "red", "darkgreen"), lty=1, cex=0.8)
dev.off()