FROM gcc:4.9

COPY . /usr/src/http-server
WORKDIR /usr/src/http-server

RUN mkdir -p build/ && cd build/ && rm -rf * 
RUN cd build/ && gcc -I.. -o HttpLinux ../server_linux.cpp ../http_tcpServer_linux.cpp -lstdc++

EXPOSE 8080 