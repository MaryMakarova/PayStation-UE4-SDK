const express = require('express');
const makeRequest = require('request');

module.exports = class TokenRouter {
    constructor(globals) {
        this.globals = globals;

        this.router = express.Router();
        this._setupRouter();
    }

    getRouter() {
        return this.router;
    }

    _setupRouter() {
        this.router.post('/', (req, res, next) => {
            let url = 'https://' 
            + this.globals.merchantId + ':' + this.globals.apiKey 
            + '@api.xsolla.com/merchant/v2/merchants/' 
            + this.globals.merchantId 
            + '/token';

            let options = {
                url: url,
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Content-Length': req.body.length
                },
                json: JSON.parse(req.rawBody)
            };

            makeRequest(options, (err, tokenRes, body) => {
                if (err) throw err;

                if (tokenRes) {
                    this.globals.userIdList.push( JSON.parse(req.rawBody).user.id.value );

                    res.end(body.token);
                }
            });
        });
    }
};