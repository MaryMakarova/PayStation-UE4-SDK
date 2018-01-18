const express = require('express');

module.exports = class PaymentRouter {
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
            res.statusCode = this.globals.errorStatusCode;

            if (req.rawBody != '') {
                let external_id = req.rawBody;

                for (let payment of this.globals.payments) {
                    if (payment.transaction.external_id == external_id) {
                        res.statusCode = this.globals.successStatusCode;
                        res.write(JSON.stringify(payment));
                        break;
                    }
                }
            }

            res.end();
        });
    }
};