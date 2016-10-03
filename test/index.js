var fs   = require("fs");
var path = require("path");
var util = require("util");

var chai       = require("chai");
var underscore = require("underscore");
var expect     = chai.expect;

var quirc = require("../index.js");

// QR-code versions.
var qr_versions = underscore.range(1, 40);
// QR-code ECC levels.
var qr_ecc_levels = {
    ECC_LEVEL_M: "M",
    ECC_LEVEL_L: "L",
    ECC_LEVEL_H: "H",
    ECC_LEVEL_Q: "Q",
};
// QR-code encoding modes.
var qr_enc_modes = {
    MODE_NUMERIC: "NUMERIC",
    MODE_ALPHA:   "ALPHA",
    MODE_BYTE:    "BYTE",
    MODE_KANJI:   "KANJI",
};

/* helpers for test data files */
function test_data_path(local_path) {
    return path.join(__dirname, "data", local_path);
}
function read_test_data(local_path) {
    return fs.readFileSync(test_data_path(local_path));
}

describe("constants", function () {
    describe("QR-code versions", function () {
        it("should set VERSION_MIN to 1", function () {
            expect(quirc.constants.VERSION_MIN).to.exist.and.to.eql(1);
        });
        it("should set VERSION_MAX to 40", function () {
            expect(quirc.constants.VERSION_MAX).to.exist.and.to.eql(40);
        });
    });

    describe("QR-code ECC levels", function () {
        underscore.each(qr_ecc_levels, function (value, key) {
            it("should set " + key + " to " + value, function () {
                expect(quirc.constants[key]).to.exist.and.to.eql(value);
            });
        });
    });

    describe("QR-code encoding modes", function () {
        underscore.each(qr_enc_modes, function (value, key) {
            it("should set " + key + " to " + value, function () {
                expect(quirc.constants[key]).to.exist.and.to.eql(value);
            });
        });
    });
});

describe("decode()", function () {
    describe("arguments", function () {
        it("should throw an Error when no arguments are given", function () {
            expect(function () {
                quirc.decode();
            }).to.throw(Error, "expected (img, callback) as arguments");
        });
        it("should throw an Error when only one argument is given", function () {
            expect(function () {
                quirc.decode(new Buffer(""));
            }).to.throw(Error, "expected (img, callback) as arguments");
        });
        it("should throw when img is not a Buffer", function () {
            expect(function () {
                quirc.decode("a string", function dummy() { });
            }).to.throw(TypeError, "img must be a Buffer");
        });
        it("should throw when callback is not a function", function () {
            expect(function () {
                quirc.decode(new Buffer(""), "not a function");
            }).to.throw(TypeError, "callback must be a function");
        });
    });

    context("when the buffer data is not an image", function () {
        it("should yield an Error", function (done) {
            quirc.decode(new Buffer(""), function (err, codes) {
                expect(err).to.exist.and.to.be.an("error");
                expect(err.message).to.eql("failed to load image");
                return done();
            });
        });
    });

    context("when the image file has no QR Code", function () {
        var empty_image;
        before(function () {
            empty_image = read_test_data("1x1.png");
        });

        it("should not yield an Error", function (done) {
            quirc.decode(empty_image, function (err, codes) {
                expect(err).to.not.exist;
                return done();
            });
        });
        it("should not yield a result", function (done) {
            quirc.decode(empty_image, function (err, codes) {
                expect(codes).to.be.an('array').and.to.have.length(0);
                return done();
            });
        });
    });

    context("when the image file has multiple QR Code", function () {
        var image_with_two_qrcodes;
        before(function () {
            image_with_two_qrcodes = read_test_data("Hello+World.png");
        });

        it("should not yield an Error", function (done) {
            quirc.decode(image_with_two_qrcodes, function (err, codes) {
                expect(err).to.not.exist;
                return done();
            });
        });
        it("should yield two results", function (done) {
            quirc.decode(image_with_two_qrcodes, function (err, codes) {
                expect(codes).to.be.an('array').and.to.have.length(2);
                return done();
            });
        });
        it("should yield the first QR Code", function (done) {
            quirc.decode(image_with_two_qrcodes, function (err, codes) {
                expect(codes[0].err).to.not.exist;
                expect(codes[0].version).to.eql(1);
                expect(codes[0].ecc_level).to.eql("L");
                expect(codes[0].mask).to.eql(0);
                expect(codes[0].mode).to.eql("BYTE");
                expect(codes[0].data).to.be.an.instanceof(Buffer);
                expect(codes[0].data.toString()).to.eql("Hello");
                return done();
            });
        });
        it("should yield the second QR Code", function (done) {
            quirc.decode(image_with_two_qrcodes, function (err, codes) {
                expect(codes[1].err).to.not.exist;
                expect(codes[1].version).to.eql(1);
                expect(codes[1].ecc_level).to.eql("L");
                expect(codes[1].mask).to.eql(7);
                expect(codes[1].mode).to.eql("BYTE");
                expect(codes[1].data).to.be.an.instanceof(Buffer);
                expect(codes[1].data.toString()).to.eql("World");
                return done();
            });
        });
    });

    context("when the image file has one QR Code", function () {
        var mode_to_data = {
            NUMERIC: "42",
            ALPHA:   "AC-42",
            BYTE:    "GET LAMP",
            KANJI:   new Buffer([0x93, 0x5f,0xe4, 0xaa]).toString(),
        };

        // generate tests for a version/ecc_level/mode combination
        function generate_single_tests(fpath, version, ecc_level, mode) {
            var image;
            before(function () {
                image = read_test_data(fpath);
            });
            it("should not yield an Error", function (done) {
                quirc.decode(image, function (err, codes) {
                    expect(err).to.not.exist;
                    return done();
                });
            });
            it("should yield one results", function (done) {
                quirc.decode(image, function (err, codes) {
                    expect(codes).to.be.an('array').and.to.have.length(1);
                    return done();
                });
            });
            it("should yield the QR Code", function (done) {
                quirc.decode(image, function (err, codes) {
                    expect(codes[0].version).to.eql(version);
                    expect(codes[0].ecc_level).to.eql(ecc_level);
                    expect(codes[0].mode).to.eql(mode);
                    expect(codes[0].data).to.be.an.instanceof(Buffer);
                    expect(codes[0].data.toString()).to.eql(mode_to_data(mode));
                    return done();
                });
            });
        }

        underscore.each(qr_versions, function (version) {
            context("version=" + version, function () {
                underscore.each(qr_ecc_levels, function (ecc_level) {
                    context("ecc_level=" + ecc_level, function () {
                        underscore.each(qr_enc_modes, function (mode) {
                            context("mode=" + mode, function () {
                                var fpath = util.format(
                                    "generated/version=%d,level=%s,mode=%s.png",
                                    version, ecc_level, mode
                                 );
                                fs.access(test_data_path(fpath), fs.R_OK, function (err) {
                                    if (err) {
                                        it.skip(fpath + " not generated");
                                    } else {
                                        generate_single_tests(fpath, version, ecc_level, mode);
                                    }
                                });
                            });
                        });
                    });
                });
            });
        });
    });
});
