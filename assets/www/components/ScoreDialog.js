var score_dialog_template = /*template*/`
<v-dialog v-model="show" fullscreen hide-overlay transition="dialog-bottom-transition" scrollable>
<v-card tile>
  <v-toolbar card dark color="primary">
    <v-btn icon dark @click.stop="show=false">
      <v-icon>close</v-icon>
    </v-btn>
    <v-toolbar-title>Score edit</v-toolbar-title>
    <v-spacer></v-spacer>
    <v-toolbar-items>
      <v-btn dark flat @click.stop="save">Save</v-btn>
    </v-toolbar-items>
  </v-toolbar>

  <!-- CENTRE DU DIALOG --> 
  <v-card-text>
      <v-flex xs12 sm6 md3>
        <span>{{t1}}</span>

        <v-text-field v-model="t1ScoreClone" type="text" :rules="[(value) => value <= 13 || 'Max 13 points']" :mask="mask" label="Number" append-outer-icon="add" @click:append-outer="incrementTeam1" prepend-icon="remove" @click:prepend="decrementTeam1"></v-text-field>

      <v-divider></v-divider>

      <span>{{t2}}</span>

        <v-text-field v-model="t2ScoreClone" type="text" :rules="[(value) => value <= 13 || 'Max 13 points']" :mask="mask" label="Number" append-outer-icon="add" @click:append-outer="incrementTeam2" prepend-icon="remove" @click:prepend="decrementTeam2"></v-text-field>

    </v-flex>
  </v-card-text>
 
</v-card>
</v-dialog>`

ScoreDialog = {
  template: score_dialog_template,
  props: {
	  visible: {
		  type: Boolean,
		  default: false
	  },
	  round: {
		  type: Number,
		  default: 0
	  },
	  t1: {
		  type: String,
		  default: ''
	  },
	  t2: {
		  type: String,
		  default: ''
	  },
	  t1score: {
		  type: Number,
		  default: 0
	  },
	  t2score: {
		  type: Number,
		  default: 0
	  },
	  t1id: {
		  type: Number,
		  default: 0
	  },
	  t2id: {
		  type: Number,
		  default: 0
	  }
  },
  //====================================================================================================================
  data() {
      return {
        errorMessages: '',
        mask: '##',
        t1ScoreClone: this.t1score,
        t2ScoreClone: this.t2score
      }
  },
  //====================================================================================================================
  computed: {
    show: {
      get () {
        return this.visible
      },
      set (value) {
        if (!value) {
          this.$emit('close');
        }
      }
    }
  },
  //====================================================================================================================
  methods: {

    incrementTeam1 () {
		if (this.t1ScoreClone < 13) {
				this.t1ScoreClone = parseInt(this.t1ScoreClone, 10) + 1;
		}
    },
    decrementTeam1 () {
		if (this.t1ScoreClone > 0) {
				this.t1ScoreClone = parseInt(this.t1ScoreClone, 10) - 1;
		}
    },
    incrementTeam2 () {
		if (this.t2ScoreClone < 13) {
				this.t2ScoreClone = parseInt(this.t2ScoreClone, 10) + 1;
		}
    },
    decrementTeam2 () {
		if (this.t2ScoreClone > 0) {
				this.t2ScoreClone = parseInt(this.t2ScoreClone, 10) - 1;
		}
    },
    isScoreOk(score) {
		let success = false;
		if ((score >= 0) && (score <= 13)) {
		   success = true; 
		}
		return success;
    },
    save: function() {

      let formHasErrors = false;
      let scores = {
        round: this.round,
        team1Score: parseInt(this.t1ScoreClone),
        team2Score: parseInt(this.t2ScoreClone),
        team1Id: this.t1id,
        team2Id: this.t2id
      };

      if (this.isScoreOk(scores.team1Score) && this.isScoreOk(scores.team2Score)) {

        this.$store.dispatch('setScores', scores).then( (doc) => {
          return console.log('[SCORES] Successfully saved the score!');
        }).catch((err) => {
            console.log('[SCORES] Save score failure: ' + err);
            this.$eventHub.$emit('alert', "Erreur: impossible de sauvegarder les scores", 'error');
        });
        
      	this.$emit('close');  
      } else {
          console.log('[SCORES] Score not valid');
          this.$eventHub.$emit('alert', "Erreur: les scores ne sont pas valides", 'error');
      }
      
    }


  }
}


