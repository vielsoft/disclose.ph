<?php
/**
 * @file
 * Hooks provided by Masonry Fields.
 */

/**
 * Specify field types and their formatters that can be displayed in a Masonry
 * layout.
 *
 * @return
 *   An associative array where the keys are field types and the values are
 *   arrays of formatter type names.
 */
function hook_masonry_fields_field_types() {
  return array(
    'image' => array(
      'image',
    ),
    'text_long' => array(
      'text_default',
      'text_plain',
      'text_trimmed',
    ),
    'text_with_summary' => array(
      'text_default',
      'text_plain',
      'text_summary_or_trimmed',
      'text_trimmed',
    ),
  );
}

