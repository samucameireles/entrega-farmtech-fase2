import sys

try:
    import requests
except ImportError:
    print("Instale a dependencia antes de rodar: pip install requests")
    sys.exit(1)


def buscar_previsao_chuva(cidade="Cacapava, SP"):

    resp_geo = requests.get(
        "https://geocoding-api.open-meteo.com/v1/search",
        params={"name": cidade, "count": 1, "language": "pt", "format": "json"},
        timeout=10,
    )
    dados_geo = resp_geo.json()

    if "results" not in dados_geo:
        raise ValueError(f"Cidade '{cidade}' nao encontrada.")

    lat = dados_geo["results"][0]["latitude"]
    lon = dados_geo["results"][0]["longitude"]
    nome = dados_geo["results"][0]["name"]

    resp_clima = requests.get(
        "https://api.open-meteo.com/v1/forecast",
        params={
            "latitude": lat,
            "longitude": lon,
            "daily": "precipitation_sum",
            "timezone": "America/Sao_Paulo",
            "forecast_days": 1,
        },
        timeout=10,
    )
    dados_clima = resp_clima.json()
    chuva_mm = dados_clima["daily"]["precipitation_sum"][0]

    return nome, chuva_mm


def tentar_enviar_via_serial(comando):
    try:
        import serial
        import serial.tools.list_ports
    except ImportError:
        return False

    portas = list(serial.tools.list_ports.comports())
    if not portas:
        return False

    porta = portas[0].device
    try:
        with serial.Serial(porta, 115200, timeout=2) as ser:
            ser.write((comando + "\n").encode("utf-8"))
        print(f"-> Comando enviado automaticamente pela porta serial {porta}.")
        return True
    except Exception as erro:
        print(f"-> Nao foi possivel usar a porta serial {porta}: {erro}")
        return False


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    cidade = args[0] if args else "Cacapava, SP"

    print("=========================================================")
    print(" FARMTECH SOLUTIONS - Previsao de chuva para a irrigacao")
    print("=========================================================\n")

    print(f"Buscando previsao para: {cidade} ...")
    nome, chuva_mm = buscar_previsao_chuva(cidade)
    print(f"Local encontrado: {nome}")
    print(f"Chuva prevista para hoje: {chuva_mm:.1f} mm\n")

    comando = f"CHUVA:{chuva_mm:.1f}"

    print(">>> Cole esta linha no Monitor Serial do Wokwi e de Enter: <<<\n")
    print(f"    {comando}\n")
    print("O ESP32 usa esse valor para decidir se liga ou nao a bomba.")

    if "--serial" in sys.argv:
        tentar_enviar_via_serial(comando)


if __name__ == "__main__":
    main()
