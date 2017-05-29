FROM node:7.10

RUN mkdir code
WORKDIR code
ADD server.js .
RUN npm install coap
RUN npm install influx

EXPOSE 5683

CMD node server.js

# sudo dockerd --ipv6 --fixed-cidr-v6="2001:470:1::/64"