FROM node:7.10

RUN mkdir code
WORKDIR code
ADD server.js .
RUN npm install coap

EXPOSE 8888

CMD node server.js