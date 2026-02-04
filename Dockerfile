FROM ubuntu:24.04

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y \
        g++-14 \
        cmake \
        python3 \
        python3-pip \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# setup conan
ENV CXX=g++-14

RUN pip install conan --break-system-packages
RUN conan profile detect --force

# build
RUN conan install . --output-folder=build --build=missing
RUN cmake --preset conan-release
RUN cmake --build --preset conan-release

CMD ["./build/fitbit"]
