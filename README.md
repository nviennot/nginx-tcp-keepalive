# Nginx TCP keepalive module

When using [Nginx](http://wiki.nginx.org/Main) as a reverse proxy where the
connections stay open (for example, to implement a server push),
a problem arise when the client times out: nginx doesn't detect it because it
doesn't use the [TCP keepalive](http://tldp.org/HOWTO/TCP-Keepalive-HOWTO/overview.html) mechanism.
The fact that nginx doesn't detect connection timeouts implies that the backend server
(used for comet, server push, etc..) will see the client as connected forever which is
bad for two reasons: 1) the client presence can matter in your application,
and 2) resources will never get freed up.

Note: this has nothing to do with HTTP keepalive.

This module allows you to set the TCP keepalive values on http connections, namely
`TCP_KEEPCNT`, `TCP_KEEPIDLE`, `TCP_KEEPINTVL`.

## Installation

### From source

Configure the Nginx sources with:

    ./configure --add-module=path/to/nginx_tcp_keepalive

And then compile/install with make/make install.

### On Ubuntu

Add the following PPA and install nginx:

    sudo add-apt-repository ppa:nviennot/nginx
    sudo apt-get update
    sudo apt-get install nginx

## Configuration Options

### tcp_keepalive

* **syntax**: tcp\_keepalive *on|off*
* **default**: off
* **context**: http, server, location

Enables the TCP keepalive mechanism.

---

### tcp_keepcnt

* **syntax**: tcp\_keepcnt *num*
* **default**: 2
* **context**: http, server, location

The maximum number of keepalive probes TCP should send before dropping the connection.

---

### tcp_keepidle

* **syntax**: tcp\_keepidle *time*
* **default**: 1m
* **context**: http, server, location

The time the connection needs to remain idle before TCP starts sending keepalive probes.

---

### tcp_keepintvl

* **syntax**: tcp\_keepintvl *time*
* **default**: 1m
* **context**: http, server, location

The time between individual keepalive probes.

## License

nginx_tcp_keepalive is distributed under the MIT license.
