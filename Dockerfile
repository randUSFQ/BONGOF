# Imagen oficial de Ubuntu optimizada por el CERN para análisis científico
FROM rootproject/root:latest

# Instalar herramientas de compilación y descarga de datos del CERN como root
USER root
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    wget \
    git \
    && rm -rf /var/lib/apt/lists/*

# Establecer el directorio de trabajo
WORKDIR /analysis

# En lugar de crear un usuario con UID 1000, creamos uno con UID 1005 para evitar colisiones
RUN useradd -u 1005 -m physics_phd && chown -R physics_phd:physics_phd /analysis
USER physics_phd

# Comando por defecto: abrir la consola interactiva de ROOT en modo silencioso
CMD ["root", "-l"]
