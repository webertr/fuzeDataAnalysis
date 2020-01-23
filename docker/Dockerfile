FROM webertr/fuze:latest
#FROM debian

#RUN apt-get update
#RUN apt-get install -y \
#    wget \
#    make \
#    gcc \
#    g++ \
#    libreadline-dev

RUN apt-get update
RUN apt-get install -y \
    libjpeg-dev

#WORKDIR /tmp


#COPY install.sh ./

#RUN chmod +x install.sh
#RUN ./install.sh

#COPY epics.sh /etc/profile.d/
#RUN chmod +x /etc/profile.d/epics.sh
#RUN echo ". /etc/profile.d/epics.sh" >> /etc/bash.bashrc
#RUN arch | xargs -i@ echo "/usr/local/epics/base/lib/linux-@" > /etc/ld.so.conf.d/epics.conf



#WORKDIR /home/fuze/Github

#WORKDIR /usr/local/epics

CMD ["/bin/bash"]