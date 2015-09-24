library(ggplot2)
library(bear)
library(Rmisc)

files <- c("../result/best_seller/best_seller.elapsed.csv", "../result//civil_war//battle_commander.elapsed.csv",
           "../result/company/president_company_no_keyperson_all.elapsed.csv", "../result/city/capital_state_all.elapsed.csv",
           "../result/city/city_capital_all.elapsed.csv", "../result/president/president.elapsed.csv")

dat <- rbind.fill(lapply(files, read.csv))

simrank <- rbind.fill(lapply(c("../facts/best_seller.simrank.csv",
                               "../facts/city_capital.simrank.csv",
                               "../facts/civil_war.simrank.csv",
                               "../facts/company_president.simrank.csv",
                               "../facts/president.simrank.csv",
                               "../facts/state_capital.simrank.csv"), read.csv, header=F))[,4]

titlem <- list()
titlem[["aa"]] = "AA"
titlem[["amie"]] = "AMIE"
titlem[["katz"]] = "Katz"
titlem[["sp"]] = "SP"
titlem[["pa"]] = "PA"
titlem[["ppr"]] = "PPR"
titlem[["pcrw"]] = "PCRW"
titlem[["test"]] = "Predicate\nPath"


dat$method <- unlist(lapply(dat$method, function(x){
  return (titlem[[x]])
}))

dat.se <- summarySE(data = dat, measurevar = "elapsed", groupvars = c("method"))
simrank.se <- STDERR(simrank) / 1000
dat.se <- rbind(dat.se, data.frame(method="SimRank",N=6,elapsed=simrank.se[2],sd=0,se=simrank.se[1]-simrank.se[2],ci=0))
g <- ggplot(dat.se[order(dat.se$elapsed),], aes(y=elapsed, x=method)) + 
  geom_point(size=3) + 
  geom_errorbar(aes(ymax=elapsed+ci, ymin=elapsed-ci), width=.5) + 
  scale_y_log10(expand=c(0,0.1)) + 
  scale_x_discrete(expand=c(0.05,0.05)) +
  ylab("Execution Time Per Query (second)") +
  xlab("Algorithm") +
  guides(color=guide_legend(override.aes = list(size=0.5))) +
  theme_classic() +
  theme(panel.background = element_rect(colour = "black", size=1),
        legend.justification=c(0,0),legend.title=element_blank(),
        legend.position=c(0.2,0))

ggsave("../result/time.eps", g, width=7, height=5)

