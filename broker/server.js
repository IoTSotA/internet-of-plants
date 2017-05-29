const coap    = require('coap') // or coap
    , server  = coap.createServer({type:'udp6'})
const os = require('os')


const Influx = require('influx');

const influx = new Influx.InfluxDB({
  host: 'influxdb',
  database: 'iop',
  schema: [
    {
      measurement: 'radio_data',
      fields: {
        airhum: Influx.FieldType.INTEGER,
        soilhum: Influx.FieldType.INTEGER,
        temp: Influx.FieldType.INTEGER,
        light: Influx.FieldType.INTEGER
      },
      tags: [
        'id'
      ]
    }
  ]
})

influx.getDatabaseNames()
  .then(names => {
      console.log(names);
    if (!names.includes('iop')) {
      return influx.createDatabase('iop')
    }
})


function process_data(req, res)
{
    var data = [0,0,0,0];
    var input = req.payload.slice(4,12);
    var id = req.payload.slice(0,4).join("");
    var tag;
    console.log(req.payload);
    if(id == "3620325531")
    {
        tag = "Mendel";
    }
    else
    {
        tag = "Darwin";
    }

    console.log(tag);
    for(i=0;i<4;i++)
    {
        data[i] = input[2*i]*255 + input[2*i+1]; 
    }
    console.log(data);
    influx.writePoints([
      {
        measurement: 'radio_data',
        tags: { id: tag },
        fields: { temp: data[0], airhum: data[1], light: data[2], soilhum: data[3] },
      }
    ]).then(() => {
      return influx.query(`
        select * from radio_data
        where id = ${Influx.escape.stringLit(os.hostname())}
        order by time desc
        limit 1
      `)
    }).then(rows => {
      rows.forEach(row => console.log(`Temp: ${row.temp}.\n Air hum: ${row.airhum}. \n Light: ${row.light}.\n Soil humidity: ${row.soilhum}.\n\n`))
    })
}
server.on('request', function(req, res) {
  process_data(req, res);
  res.end('OK')
})

server.listen(function() {
  console.log('server started')
})
