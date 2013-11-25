<?php

class ItonicsRelationsTR3 {
  
  
  /**
   * Create an default instance of relation
   * 
   */
  public function instance() {
    $relation = new stdClass();
    $relation->eid = '';
    $relation->entity_type = '';
    $relation->entity_bundle = '';
    $relation->eid_second = '';
    $relation->entity_type_second = '';
    $relation->entity_bundle_second = '';
    $relation->type = 'passable';
    return $relation;
  }
  
  /**
   * Save or update the relation
   * 
   * @param type $relation
   */
  public function save($relation) {
    $transaction = db_transaction();
    //
    try {
      if (empty($relation->created)) {
				$relation->created = REQUEST_TIME;
			}
      $relation->changed = REQUEST_TIME;
      $relation->is_new = (empty($relation->id)) ? true : false;
     
      if ($relation->is_new) {
        drupal_write_record('easy_relations', $relation);
        module_invoke_all('entity_insert', $relation, 'easy_relations');
      } else {
        drupal_write_record('easy_relations', $relation, 'id');
        module_invoke_all('entity_update', $relation, 'easy_relations');
      }
      // all done now
      unset($relation->is_new);
      db_ignore_slave();
			return $relation;
    } catch (Exception $e) {
			$transaction->rollback();
			watchdog_exception(ITONICS_BUSINESS_UNITS_ENTITY, $e, NULL, WATCHDOG_ERROR);
			return FALSE;
		}
  }
  
  
  /**
   * Delete relation by its ID
   * 
   * @param $id
   */
  public function delete_relation($id) {
    $id = db_delete('easy_relations')
            ->condition('id', $id)
            ->execute();
    return $id;
  }
  
  /**
   * Delete relation for given entity ID of given type
   * 
   * @param $eid
   * @param $type
   * @param $eid_second
   * @param $type_second
   */
  public function delete_for($eid, $type, $eid_second, $type_second) {
    $query = db_delete('easy_relations');
    
    $is_first_ent = db_and();
    $is_first_ent->condition('eid', $eid);
    $is_first_ent->condition('entity_type', $type);
    $is_first_ent->condition('eid_second', $eid_second, 'IN');
    $is_first_ent->condition('entity_type_second', $type_second);
    
    $is_second_ent = db_and();
    $is_second_ent->condition('eid', $eid_second, 'IN');
    $is_second_ent->condition('entity_type', $type_second);
    $is_second_ent->condition('eid_second', $eid);
    $is_second_ent->condition('entity_type_second', $type);
    
    $is_entity = db_or();
    $is_entity->condition($is_first_ent);
    $is_entity->condition($is_second_ent);
    
    
    $query->condition($is_entity);
    $id = $query->execute();
    
    return $id;
  }
  
  /**
   * Delete relation by its ID
   * 
   * @param $id_list
   */
  public function delete_all_relation($id_list) {
    $id_list = db_delete('easy_relations')
            ->condition('id', $id_list, 'IN')
            ->execute();
    return $id_list;
  }
  
  
  /**
   * Get the relation between given entities
   * 
   * @param int $eid
   * @param int $type
   * @param int $eid_second
   * @param int $entity_type_second
   */
  public function get_relation($eid, $type, $eid_second=NULL, $entity_type_second=NULL) {
    $query = db_select('easy_relations', 'rel')
            ->fields('rel');
    $db_and = db_and();
    $db_and->condition('eid', $eid);
    $db_and->condition('entity_type', $type);
    
    $db_ands = db_and();
    $db_ands->condition('eid_second', $eid);
    $db_ands->condition('entity_type_second', $type);
    
    $db_or = db_or();
    $db_or->condition($db_and);
    $db_or->condition($db_ands);
    
    $query->condition($db_or);
    
    // search also with second entity
    if (isset($eid_second)) {
      $is_other = db_or();
      $is_other->condition('eid', $eid_second);
      $is_other->condition('eid_second', $eid_second);
      $query->condition($is_other);
    }
    $result = $query->execute();
    $results = $result->fetchAll();
    
    return (count($results) > 0) ? $results : NULL;
  }
  
  /**
   * Get the relation for given entity to given second entity type
   * eg. relations for 'trend' of id '2' with all entity of type 'itonics_area'
   * 
   * @param int $eid
   * @param int $type
   * @param int $entity_type_second
   */
  public function get_relation_with_type($eid, $type, $entity_type_second) {
    $query = db_select('easy_relations', 'rel')
            ->fields('rel');
    $db_and = db_and();
    $db_and->condition('eid', $eid);
    $db_and->condition('entity_type', $type);
    
    $db_ands = db_and();
    $db_ands->condition('eid_second', $eid);
    $db_ands->condition('entity_type_second', $type);
    
    $db_or = db_or();
    $db_or->condition($db_and);
    $db_or->condition($db_ands);
    
    $query->condition($db_or);
    
    // search also with second entity
    $is_other = db_or();
    $is_other->condition('entity_type', $entity_type_second);
    $is_other->condition('entity_type_second', $entity_type_second);
    $query->condition($is_other);
      
    $result = $query->execute();
    $results = $result->fetchAll();
    
    return (count($results) > 0) ? $results : NULL;
  }
  
  /**
   * Get relation for given entity of given type
   * 
   * @param $eid
   * @param $type
   */
  public function get_any_relation($eid, $type, $bundle=NULL) {
    $query = db_select('easy_relations', 'rel')
            ->fields('rel');
    $db_or = db_or();
    $db_or->condition('eid', $eid);
    $db_or->condition('eid_second', $eid);
    
    $db_ors = db_or();
    $db_ors->condition('entity_type', $type);
    $db_ors->condition('entity_type_second', $type);
    
    $query->condition($db_or);
    $query->condition($db_ors);
    
    $result = $query->execute();
    $results = $result->fetchAll();
    $list = array();
    foreach ($results as $item) {
      $list[] = $item->id;
    }
    
    return (count($list) > 0) ? $list : NULL;
  }
  
  /**
   * Get all relations
   * 
   */
  public function get_all_relations() {
    $query = db_select('easy_relations', 'rel')
            ->fields('rel');
    $result = $query->execute();
    return $result->fetchAll();
  }
  
  /**
   * Get all entity type with relations
   * 
   */
  public function get_all_entity_types() {
    $query = db_select('easy_relations', 'rel')
            ->fields('rel', array('id', 'entity_type', 'entity_type_second'));
    $result = $query->execute();
    return $result->fetchAll();
  }
  
  /**
   * Check if relation exists between these two entities
   * 
   * @param int $eid
   * @param int $type
   * @param int $eid_second
   * @param int $type_second
   */
  public function has_relation($eid, $type, $eid_second, $type_second) {
    $query = db_select('easy_relations', 'rel')
            ->fields('rel', array('id', 'eid', 'entity_type', 'eid_second', 'entity_type_second'));
    $query->condition('eid', $eid);
    $query->condition('entity_type', $type);
    $query->condition('eid_second', $eid_second);
    $query->condition('entity_type_second', $type_second);
    $result = $query->execute();
    $results = $result->fetchAll();
    
    return (count($results) > 0) ? true : NULL;
  }
  
  /**
   * Get the other relative.
   * 
   * @param int $eid
   * @param string $type
   */
  public function get_relative($eid, $type) {
    $query = db_select('easy_relations', 'rel')
            ->fields('rel', array('id', 'eid', 'eid_second', 'entity_type'));
    $query->condition('eid', $eid);
    $query->condition('entity_type', $type);
    $result = $query->execute();
    $results = $result->fetchAll();
    
    return (count($results) > 0) ? $results[0]->eid_second : NULL;
  }
  
  /**
   * Get the other relatives.
   * 
   * @param int $eid
   * @param string $type
   */
  public function get_relatives($eid, $type, $type_second=NULL) {
    $query = db_select('easy_relations', 'rel')
            ->fields('rel', array('id', 'eid', 'entity_type', 'eid_second', 'entity_type_second'));
    $db_and = db_and();
    $db_and->condition('eid', $eid);
    $db_and->condition('entity_type', $type);
    if ($type_second) {
      $db_and->condition('entity_type_second', $type_second);
    }
    
    $db_ands = db_and();
    $db_ands->condition('eid_second', $eid);
    $db_ands->condition('entity_type_second', $type);
    if ($type_second) {
      $db_ands->condition('entity_type', $type_second);
    }
    
    $db_or = db_or();
    $db_or->condition($db_and);
    $db_or->condition($db_ands);
    
    $query->condition($db_or);
    
    $result = $query->execute();
    $results = $result->fetchAll();
    
    return (count($results) > 0) ? $results : NULL;
  }
  
  /**
   * Get eids from relation
   * @param type $type
   * @param type $eid_second
   * @param type $entity_type_second
   * @return type
   */
  public function get_eids_from_relation($type, $eid_second, $entity_type_second) {
        $query = db_select('easy_relations', 'rel')
                ->fields('rel', array('eid'));

        $query->condition('entity_type', $type);
        $query->condition('eid_second', $eid_second);
        $query->condition('entity_type_second', $entity_type_second);

        $results = $query->execute()->fetchCol();

        return (count($results) > 0) ? $results : NULL;
  }
  
  public function delete_relation_from_eids($eid_list, $type, $eid_second, $entity_type_second) {
        $query = db_delete('easy_relations')
                ->condition('eid', $eid_list, 'IN');

        $query->condition('entity_type', $type);
        $query->condition('eid_second', $eid_second);
        $query->condition('entity_type_second', $entity_type_second);
        $results = $query->execute();

        return $results;
    }
  
  
}