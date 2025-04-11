# nn
Neural Network Training

## Prerquesites

### Packages
sudo apt install build-essential libstb-dev

### Web server proxy
- Add reserve proxy for you site to 4569.  Here is a apache2 site config file:
```
<IfModule mod_proxy.c>
	ProxyRequests Off
	ProxyPreserveHost On
	<Proxy /nn>
		Order deny,allow
		Allow from all
	</Proxy>
	ProxyPass /nn http://127.0.0.1:4569/nn
	ProxyPassReverse /nn http://127.0.0.1:4569/nn
</IfModule>
```
- For nginx you should be able to just add this:
```
location /nn/ {
		proxy_pass http://127.0.0.1:4569/nn;
	}
```

- Add the directory `web` as site you can hit for your http server.
- Quick Test of proxy: 
    - start nn
	- execute this: `curl -H 'Content-Type: application/json' -d '{"cmd":"TESTING"}' -X POST http://127.0.0.1/nn/`
	- Should return this error: `{"cmd":"TESTING","response":{"error":"Unknown command","info":null,"result":-1}}`

### Traning data files
There is a large MNIST number training binary file called out in `training/mnist-training-binary.json.

## Usage
- Run this in a window: `./bin/nn`
- From browser go to `http://<your-site>/nnweb`

## Config Network

# Measure

## Load Network

## Save Network

## Load Training

## Train
