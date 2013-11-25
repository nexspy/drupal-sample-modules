<?php

/**
 * Form : Select entities to look for
 * 
 */
function easy_relations_settings_form($form, $form_state) {
  $weight = 0;
  
  $all_entities = entity_get_info();
  $options = array_keys($all_entities);
  
  $form['entities'] = array(
      '#type' => 'checkboxes',
      '#options' => drupal_map_assoc($options),
      '#title' => t('What entities should we monitor?'),
      '#weight' => $weight++,
  );
  
  $form['actions'] = array('#type' => 'actions', '#weight' => $weight++);
  $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#weight' => $weight++,
      '#submit' => array('easy_relations_settings_form_submit'),
  );
  
  return $form;
}


/**
 * Submit : easy_relations_settings_form
 */
function easy_relations_settings_form_submit($form, $form_state) {
  $values = $form_state['values'];
  $input = $form_state['input'];
  
  //
  drupal_set_message(t('Success'));
}