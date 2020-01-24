FROM mdsplus-main
#FROM ubuntu:latest
#FROM webertr/fuze:latest
#FROM debian

#WORKDIR /home/fuze/Github/fuzeDataAnalysis/data
#WORKDIR /home/fuze/Github/fuzeDataAnalysis

#COPY mdsplus.sh /etc/profile.d/
#COPY mdsplus.csh /etc/profile.d/
#ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
#RUN make

#ENV PATH=$PATH:/usr/local/mdsplus/bin

#RUN ./configure
#RUN make
#RUN make install

#RUN apt-get update && apt-get install -y \
#    libcairo2-dev \
#    libgd-dev \
#    libcerf-dev

#RUN apt-get update && apt-get install -y \
#    xterm

#WORKDIR /home/fuze/Downloads/gnuplot-5.2.8


#RUN apt-get update && apt-get install -y \
#    emacs

#WORKDIR /home/fuze/Downloads/

#WORKDIR /home/fuze/Github/fuzeDataAnalysis

#RUN apt-get update && apt-get install -y \
#    libjpeg-dev

#ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/epics/base-3.15.6/lib/linux-x86_64
#ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/mdsplus/lib

#ENV MDS_PATH=/usr/local/UWmdsplus/tdi;/usr/local/mdsplus/tdi
#ENV MDSPLUS_DIR=/usr/local/mdsplus

#ENV EPICS_BASE=/usr/local/epics/base-3.15.6
#ENV EPICS_HOST_ARCH=linux-x86_64

#RUN make

#WORKDIR /home/fuze/Github/fuzeDataAnalysis
#COPY UWmdsplus.tar.gz /tmp
#COPY mdsplus.conf /etc/
#WORKDIR /tmp
#RUN tar -xf UWmdsplus.tar.gz
#RUN mv /tmp/UWmdsplus /usr/local
#RUN rm /tmp/UWmdsplus.tar.gz


#RUN apt-get update && apt-get install -y \
#    whiptail

#RUN apt-get update && apt-get install -y \
#    git \
#    libgsl-dev \
#    libxml2-dev \
#    libpqxx-dev

#WORKDIR /home/fuze/Github
#RUN git clone https://github.com/webertr/fuzeDataAnalysis.git

#RUN apt-get update && apt-get install -y \
#    curl \
#    gnupg2

#RUN curl -fsSL http://www.mdsplus.org/dist/mdsplus.gpg.key | apt-key add -
#RUN echo 'deb [arch=amd64] http://www.mdsplus.org/dist/Ubuntu18/repo MDSplus stable' | tee /etc/apt/sources.list.d/mdsplus.list
#RUN apt-get update
#RUN apt-get update && apt-get install -y \
#    mdsplus-devel \
#    mdsplus


#WORKDIR /usr/local/epics/base-3.15.6/support/seq
#RUN make
#WORKDIR /usr/local/epics/base-3.15.6/support/calc
#RUN make
#WORKDIR /usr/local/epics/base-3.15.6/support/asyn
#RUN make
#WORKDIR /usr/local/epics/base-3.15.6/support/stream
#RUN make
#WORKDIR /usr/local/epics/base-3.15.6/support/modbus
#RUN make
#WORKDIR /usr/local/epics/base-3.15.6/support/autosave
#RUN make

#RUN apt-get update && apt-get install -y \
#    dialog \
#    perl
    
#WORKDIR /usr/local/epics/base-3.15.6
#RUN mv ../support ./
#RUN export EPICS_HOST_ARCH=linux-x86_64
#RUN make

#RUN apt-get update && apt-get install -y \
#    apt-utils \
#    wget \
#    make \
#    gcc \
#    g++ \
#    libreadline-dev \
#    libnet1-dev \
#    re2c \
#    libpcap-dev \
#    libpcre3-dev \
#    perl \
#    libnet1-dev


#WORKDIR /tmp
#COPY gnuplot-5.2.8.tar.gz ./
#RUN tar -xf gnuplot-5.2.8.tar.gz
#COPY EPICS.tar.gz ./
#RUN tar -xf EPICS.tar.gz
#WORKDIR /usr/local/epics
#RUN cp -r /tmp/EPICS/base-3.15.6 ./
#WORKDIR /usr/local/epics/support
#RUN cp -r /tmp/EPICS/asyn ./
#RUN cp -r /tmp/EPICS/autosave ./
#RUN cp -r /tmp/EPICS/calc ./
#RUN cp -r /tmp/EPICS/modbus ./
#RUN cp -r /tmp/EPICS/seq ./
#RUN cp -r /tmp/EPICS/stream ./
#WORKDIR /home/fuze/Downloads/
#RUN cp -r /tmp/gnuplot-5.2.8 ./
#RUN rm -rf /tmp/gnuplot-5.2.8.tar.gz
#RUN rm -rf /tmp/EPICS.tar.gz

#WORKDIR /home/fuze/Github/fuzeDataAnalysis/data
#WORKDIR /home/fuze/Github/fuzeDataAnalysis

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

#RUN apt-get update && apt-get install -y \
#    libcairo2-dev \
#    libgd-dev \
#    libcerf-dev

#RUN apt-get update && apt-get install -y \
#    xterm

#WORKDIR /home/fuze/Downloads
#COPY gnuplot-5.2.8.tar.gz ./
#RUN tar -xf gnuplot-5.2.8.tar.gz
#WORKDIR /home/fuze/Downloads/gnuplot-5.2.8
#RUN ./configure
#RUN make
#RUN make install

#WORKDIR /usr/local/epics

CMD ["/bin/bash"]