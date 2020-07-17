Hi this graphics experiment a precursor to retroMode.library

“chunky_plains” 

  did masking and shifting on etch pixel write.
  (Some things I should say this was in P96 I’m sure wrote directly to video memory. This might not be ideal.)

“chunky_plains2” 

  used 8 buffers and mixed the result after, you drawn it.

Etch technique had their own issues, mostly it costed CPU speed one way or the other. I did not even try rearrange bits. 

In my judgment “chunk_plains” worked OK, but I did not want the automatic masking for retroMode library, as it was costly. What I ended up where set of simple pixel plot command for XOR, AND & OR logical operations, this can produce affects you most commonly associate with planar graphics, but I do not have individual buffers, and do not try mix layers, you set a palette as you normally did.
