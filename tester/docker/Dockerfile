FROM ubuntu:latest

WORKDIR /app

RUN apt-get update
RUN apt-get -y install --no-install-recommends build-essential

#RUN apk add --update --no-cache bash musl-dev make gcc git

# RUN git clone https://github.com/giolekva/gl.git

ENV GL_LIB_PATH=/app/gl
ENV CC=gcc
ENV LD='gcc'

ENV MODE='--class'

CMD  make -C /app/gl/lunit lunit \
     && make -C /app/gl/tester run_tests \
     && /app/gl/tester/run_tests $MODE --students_dir=/exam/students --problems_dir=/exam/problems --results_dir=/exam/results
