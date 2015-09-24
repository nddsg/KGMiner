dat <- read.csv("../result/city/city_capital_all.test.csv")
weights <- information.gain(label~.,dat)
weights <- data.frame(path = rownames(weights), importance=weights$attr_importance)
weights <- weights[order(weights$importance, decreasing = T),]
get_description <- function(threshold) {
	true_labeled <- colSums(dat[which(dat$label==T),])
	true_labeled <- names(true_labeled[which(true_labeled > 0)])
	false_labeled <- colSums(dat[which(dat$label==F),])
	false_labeled <- names(false_labeled[which(false_labeled > threshold)])
	idx <- !is.element(true_labeled,false_labeled)
	true_only <- true_labeled[idx]
	true_only <- true_only[which(true_only != "label")]

	np <- weights[which((weights$path %in% true_only) & weights$importance > 0),]
	return(np[order(np$importance, decreasing=T),])
}
