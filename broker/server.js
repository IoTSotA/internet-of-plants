const coap    = require('../') // or coap
    , server  = coap.createServer({type:'udp6'})

function process_data(req, res)
{
    var data = [0,0,0,0];
    var input = req.payload;
    var js = {};
    for(i=0;i<4;i++)
    {
        data[i] = input[2*i]*255 + input[2*i+1]; 
    }
    js["temp"] = data[0];
    js["air_humidity"] = data[1];
    js["light"] = data[2];
    js["soil_humidity"] = data[3];

    console.log(JSON.stringify(js));
}
server.on('request', function(req, res) {
  process_data(req, res);
  res.end('OK')
})

server.listen(function() {
  console.log('server started')
})
