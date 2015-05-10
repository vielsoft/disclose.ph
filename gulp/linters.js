'use strict';

var gulp = require('gulp');
var jscs = require('gulp-jscs');
var jshint = require('gulp-jshint');

module.exports = function (target, options) {
  var srcDir = options.srcDir + '/js';
  gulp.task('linters', function () {
    return gulp.src([
      srcDir + '/**/*.js',
      '!' + srcDir + '/vendor/**/*.js',
      '!' + srcDir + '/scripts.js',
      'gulp/**/*.js',
      'gulpfile.js'
    ]).pipe(jscs()).pipe(jshint('.jshintrc')).pipe(jshint.reporter('jshint-stylish')).pipe(jshint.reporter('fail'));
  });
};
