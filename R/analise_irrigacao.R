dados <- read.csv("leituras.csv", stringsAsFactors = FALSE)

cat("=========================================================\n")
cat(" FARMTECH SOLUTIONS - Analise estatistica da irrigacao\n")
cat("=========================================================\n\n")

cat("Total de leituras analisadas:", nrow(dados), "\n\n")

media_umidade  <- mean(dados$umidade)
desvio_umidade <- sd(dados$umidade)
media_ph  <- mean(dados$ph)
desvio_ph <- sd(dados$ph)

cat("--- Umidade do solo ---\n")
cat(sprintf("Media .........: %.2f %%\n", media_umidade))
cat(sprintf("Desvio padrao .: %.2f %%\n\n", desvio_umidade))

cat("--- pH do solo ---\n")
cat(sprintf("Media .........: %.2f\n", media_ph))
cat(sprintf("Desvio padrao .: %.2f\n\n", desvio_ph))

PH_MIN <- 6.0
PH_MAX <- 6.5
UMIDADE_MIN <- 60.0
UMIDADE_MAX <- 80.0

cat("--- Recomendacao com base no historico ---\n")

if (media_ph < PH_MIN || media_ph > PH_MAX) {
  cat("pH medio fora da faixa ideal (6.0 - 6.5): NAO IRRIGAR, corrija o solo.\n")
} else if (media_umidade < UMIDADE_MIN) {
  cat("Umidade media abaixo do ideal: RECOMENDA-SE IRRIGAR.\n")
} else if (media_umidade >= UMIDADE_MAX) {
  cat("Umidade media acima do ideal: NAO IRRIGAR (risco de excesso de agua).\n")
} else {
  cat("Umidade e pH dentro da faixa ideal: MANTER ESTADO ATUAL da bomba.\n")
}

cat("\n--- Observacao sobre variabilidade ---\n")
if (desvio_umidade > 10) {
  cat("Desvio padrao da umidade esta alto (> 10): leituras instaveis,\n")
  cat("verifique o sensor DHT22 no circuito.\n")
} else {
  cat("Leituras de umidade estaveis (desvio padrao <= 10).\n")
}

cat("\n=========================================================\n")
cat(" Fim da analise\n")
cat("=========================================================\n")
