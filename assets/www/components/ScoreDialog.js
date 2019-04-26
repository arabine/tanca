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
        <span>{{team1}}</span>

        <v-text-field v-model="team1Score" type="text" :rules="[(value) => value <= 13 || 'Max 13 points']" :mask="mask" label="Number" append-outer-icon="add" @click:append-outer="incrementTeam1" prepend-icon="remove" @click:prepend="decrementTeam1"></v-text-field>


    </v-flex>
  </v-card-text>
 
</v-card>
</v-dialog>`

ScoreDialog = {
  template: score_dialog_template,
  props: ['visible'],
  data() {
      return {
        errorMessages: '',
        formHasErrors: false,
        team1: 'Coucou',
        team2: '',
        team1Score: 0,
        team2Score: 0,
        mask: '##'
      }
  },
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
    },
    form () {
      return {
        team1Score: this.team1Score,
        team2Score: this.team2Score,
      }
    }
  },
  methods: {

    incrementTeam1 () {
        this.team1Score = parseInt(this.team1Score, 10) + 1
    },
    decrementTeam1 () {
        this.team1Score = parseInt(this.team1Score, 10) - 1
    },
    save: function() {
/*
      this.formHasErrors = false

      Object.keys(this.form).forEach(f => {
        if (!this.form[f]) this.formHasErrors = true
      });

      if (!this.formHasErrors) {
        var player = {
          firstname: this.firstname,
          lastname: this.lastname
        }

        this.$store.dispatch('addPlayer', player).then( (doc) => {
          return console.log('[PLAYERS] Successfully added a player!');
        }).catch((err) => {
            console.log('[PLAYERS] Add player failure: ' + err);
            this.$eventHub.$emit('alert', "Erreur: impossible de créer le joueur", 'error');
        });
        
        
        
      }
      */

      this.$emit('close');
    }


  }
}


