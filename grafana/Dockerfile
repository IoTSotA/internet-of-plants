from grafana/grafana

MAINTAINER Felipe Lalanne <felipe@lalanne.cl>

# Install sqlite3
RUN DEBIAN_FRONTEND=noninteractive apt-get update && apt-get -q -y install sqlite3

# Load stored database. Use configurable path
ADD ioplants.sql /tmp
ADD grafana.sh /
RUN chmod 755 /grafana.sh


ENTRYPOINT ["/grafana.sh"]
