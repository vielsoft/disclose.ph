'use strict';

var gulp = require('gulp');
module.exports = function (target, options) {
  var srcDir = options.srcDir;

  gulp.task('watch', function () {
    gulp.watch(srcDir + '/js/**/*.js', ['scripts']);
    gulp.watch(srcDir + '/sass/**/*.scss', ['styles']);
  });
};
