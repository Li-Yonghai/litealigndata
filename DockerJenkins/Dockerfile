FROM ubuntu
MAINTAINER litealigndata litealigndata@email.com
ENV DEBIAN_FRONTEND=noninteractive
ENV LANG C.UTF-8
RUN sed -i 's@http://archive.ubuntu.com/ubuntu/@https://hub.docker.com/@g' /etc/apt/sources.list
RUN mkdir /jenkins
WORKDIR /jenkins
COPY jenkins.war ./
COPY run.sh ./
RUN chmod +x run.sh           && \
    apt-get update install gcc g++ make automake git maven -y
RUN mkdir /usr/java && mkdir /usr/maven
WORKDIR /usr/java
ADD jdk-17.0.3.tar.gz ./ 
ENV JAVA_HOME=/usr/java/jdk-17.0.3
ENV PATH=${JAVA_HOME}/bin:$PATH

expose 8080 
WORKDIR /jenkins
ENTRYPOINT [ "./run.sh" ]
