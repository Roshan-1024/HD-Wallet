# ---------- Dev Stage ----------
FROM ubuntu:22.04 AS dev

RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake \
        libssl-dev \
        gdb \
        && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

CMD ["/bin/bash"]

# ---------- Builder Stage ----------
FROM dev AS builder

RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    cmake --build . --target wallet --config Release

# ---------- Runtime Stage ----------
FROM ubuntu:22.04 AS prod

RUN apt-get update && apt-get install -y libssl-dev && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy only the built binary from the builder stage
COPY --from=builder /app/build/bin/wallet .
COPY --from=builder /app/wordlists ./wordlists

CMD ["/bin/bash"]
