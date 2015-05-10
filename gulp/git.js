'use strict';

var gulp = require('gulp');
var shell = require('gulp-shell');
module.exports = function () {
  gulp.task('git-add', function () {
    return gulp.src('*.js', {
      read: false
    }).pipe(shell(['git ls-files --others docroot/sites/all | egrep \'.css|.js|.png|.map\' | xargs git add -f']));
  });
};
