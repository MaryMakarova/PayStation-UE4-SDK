const express = require('express');

module.exports = class UserRouter {
    constructor(globals) {
        this.globals = globals;

        this.router = express.Router();
        this._setupRouter();
    }

    getRouter() {
        return this.router;
    }

    _setupRouter() {
        this.router.post('/add', (req, res, next) => {
            res.statusCode = this.globals.errorStatusCode;

            if (req.rawBody != '') {
                this.globals.userIdList.add(req.rawBody);
                res.statusCode = this.globals.successStatusCode;
            }

            res.end();
        });
    }
};