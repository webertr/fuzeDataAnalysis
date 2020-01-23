FROM webertr/fuze:latest
#FROM debian

#RUN apt-get update && apt-get install -y \
#    wget \
#    make \
#    gcc \
#    g++ \
#    libreadline-dev

#WORKDIR /tmp

#COPY install.sh ./

#RUN chmod +x install.sh
#RUN ./install.sh

#WORKDIR /tmp
#COPY mdsplus.sh /etc/profile.d/
#RUN chmod +x /etc/profile.d/mdsplus.sh
#RUN echo ". /etc/profile.d/mdsplus.sh" >> /etc/bash.bashrc

#WORKDIR /tmp
#COPY epics.sh /etc/profile.d/
#RUN chmod +x /etc/profile.d/epics.sh
#RUN echo ". /etc/profile.d/epics.sh" >> /etc/bash.bashrc
#RUN arch | xargs -i@ echo "/usr/local/epics/base/lib/linux-@" > /etc/ld.so.conf.d/epics.conf


#WORKDIR /home/fuze/Github/fuzeDataAnalysis/
#RUN git pull
#RUN make

RUN apt-get update && apt-get install -y \
    libcairo2-dev \
    libgd-dev \
    libcerf-dev

WORKDIR /home/fuze/Downloads
COPY gnuplot-5.2.8.tar.gz ./
RUN tar -xf gnuplot-5.2.8.tar.gz
WORKDIR /home/fuze/Downloads/gnuplot-5.2.8
RUN ./configure
RUN make
RUN make install

#WORKDIR /usr/local/epics

CMD ["/bin/bash"]