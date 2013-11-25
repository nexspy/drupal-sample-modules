<?php
/**
 * 
 * Available variable:
 * 
 * $total - total number of relations
 * 
 * $data - all relations loaded
 * 
 */
?>
<span><?php echo t('Results') . '(' . $total . ')'; ?></span>

<?php if ($total > 0) : ?>

<ul class="relations-map">
<?php foreach ($data as $row) : ?>

<li>
  <div class="counting-rel">
        (<?php echo $row['count']; ?>)
  </div>
  <div class="left-rel">
      <?php echo $row['first']; ?>
  </div>
  <div  class="middle-rel">
    ->
  </div>
  <div class="right-rel">
      <?php echo $row['second']; ?>
  </div>
  
</li>

<?php endforeach; ?>
</ul>


<?php else : ?>
    <?php echo t('No relations'); ?>
<?php endif; ?>

