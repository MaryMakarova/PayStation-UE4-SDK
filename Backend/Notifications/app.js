const http          = require('http');
const https         = require('https');
const makeRequest   = require('request');
const sha1          = require('sha1');
const fs            = require('fs');
const querystring   = require('querystring');

// listening port
const PORT = 3000;

// project secret key
let secretKey  = 'e31loBH0ShptLnxp';

// list of accepted users
// you can load it from database
let userIdList = ['olegididid', '12345', 'idididid'];

// successful payments
let payments = [];

// read payments from dump file
if (fs.existsSync('./payments.dump')) {
    fs.readFile('./payments.dump', (err, data) => {
        if (err) throw err;

        if (data != '') {
            payments = JSON.parse(data);
        }
    });
}

const server = http.createServer((request, response) => {

    /**
    * Handle POST request to '/notification'
    *
    * processes notifications from Xsolla
    */
    if (request.method == 'POST' && request.url == '/notification') {
        let bodyArr = [];
        let body = {};

        request.on('data', (chunk) => {
            bodyArr.push(chunk);
        }).on('end', () => {
            bodyArr = Buffer.concat(bodyArr).toString();
            body = JSON.parse(bodyArr);

            // check request sgnature
            let reqSign = request.headers.authorization;
            let encryptedSign = 'Signature ' + sha1(bodyArr + secretKey);

            // if request signature is not valid
            if (reqSign != encryptedSign) {
                response.statusCode = 400;
                response.end('{"error":{"code":"INVALID_SIGNATURE"}}');
            }

            // check is user id exists in our id array
            if (body.notification_type == 'user_validation') {
                userIdList.forEach((item, i, arr) => {
                    if (body.user.id == item) {
                        response.statusCode = 204;
                        response.end();
                    } else {
                    response.statusCode = 400;
                    response.end('{"error":{"code":"INVALID_USER"}}');
                    }
                });
            }

            // save successful payment
            if (body.notification_type == 'payment') {
                payments.push(body);

                fs.writeFile('./payments.dump', JSON.stringify(payments), (err) => {
                  if (err) throw err;
                });

                console.log('Payment succeeded, id: ' + body.transaction.id + " external_id: " + body.transaction.external_id);

                response.statusCode = 204;
                response.end();
            }

            response.statusCode = 404;
            response.end();
        });
    }

    /**
    * Handle POST request to '/payment'
    * 
    * accepts payment external_id
    *
    * returns 200 and payment JSON if payment exists, otherwise 400 
    */
    if (request.method == 'POST' && request.url == '/payment') {
        // read request body
        let bodyArr = [];
        let body = 0;

        request.on('data', (chunk) => {
            bodyArr.push(chunk);
        }).on('end', () => {
            bodyArr = Buffer.concat(bodyArr).toString();
            body = bodyArr.toString();
            
            response.statusCode = 400;

            // check is payment external_id exists in array
            payments.forEach((item, i, arr) => {
                if (body == item.transaction.external_id) {
                    response.statusCode = 200;
                    response.write(JSON.stringify(item));
                }
            });

            response.end();
        });
    }
 });

server.listen(PORT);