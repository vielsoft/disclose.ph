'use strict';

var gulp = require('gulp');
var gulpif = require('gulp-if');
var concat = require('gulp-concat');
var sourcemaps = require('gulp-sourcemaps');
var uglify = require('gulp-uglify');
module.exports = function (target, options) {
  var isProd = target === 'production';
  var srcDir = options.srcDir + '/js';
  var destDir = options.destDir + '/js';
  gulp.task('scripts', function () {
      return gulp.src([
        srcDir + '/script.js'
      ])
      .pipe(gulpif(isProd, sourcemaps.init()))
      .pipe(concat('scripts.js'))
      .pipe(gulpif(isProd, uglify()))
      .pipe(gulpif(isProd, sourcemaps.write('.', {
        sourceMappingURLPrefix: '/sites/all/themes/disclose/js/',
        includeContent: false
      })))
      .pipe(gulp.dest(destDir));
  });
};
