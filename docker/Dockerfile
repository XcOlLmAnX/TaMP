FROM ubuntu:22.04

ENV TZ=Europe/Moscow
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && apt-get install -y \
    qt6-base-dev \
    libqt6sql6-sqlite \
    libssl-dev \
    cmake \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root/server/

COPY TaMP_Server/*.cpp /root/server/
COPY TaMP_Server/*.h   /root/server/
COPY TaMP_Server/CMakeLists.txt /root/server/

# email.txt должен существовать локально перед запуском docker build.
# Создай его: cp TaMP_Server/email.txt.example TaMP_Server/email.txt
# Затем заполни: email = your@mail.ru  и  key = app_password
COPY TaMP_Server/email.txt /root/server/email.txt

RUN cmake -B build && cmake --build build

EXPOSE 12345

ENTRYPOINT ["./build/TaMP"]
