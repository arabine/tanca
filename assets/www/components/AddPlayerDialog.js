var add_player_dialog_template = /*template*/`
<v-dialog v-model="show" fullscreen hide-overlay transition="dialog-bottom-transition" scrollable>
<v-card tile>
  <v-toolbar card dark color="primary">
    <v-btn icon dark @click.stop="show=false">
      <v-icon>close</v-icon>
    </v-btn>
    <v-toolbar-title>Add player</v-toolbar-title>
    <v-spacer></v-spacer>
    <v-toolbar-items>
      <v-btn dark flat @click.stop="savePlayer">Save</v-btn>
    </v-toolbar-items>
  </v-toolbar>

  <!-- CENTRE DU DIALOG --> 
  <v-card-text>
      <v-flex xs12 sm6 md3>
        <v-text-field v-model="first_name" :rules="[() => !!first_name || 'This field is required']" :error-messages="errorMessages" label="Firt name" required></v-text-field>
        <v-text-field v-model="last_name" :rules="[() => !!last_name || 'This field is required']" :error-messages="errorMessages" label="Last name" required></v-text-field>
    </v-flex>
  </v-card-text>
 
</v-card>
</v-dialog>`

AddPlayerDialog = {
  template: add_player_dialog_template,
  props: ['visible'],
  data() {
      return {
        errorMessages: '',
        formHasErrors: false,
        first_name: null,
        last_name: null,
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
        first_name: this.first_name,
        last_name: this.last_name,
      }
    }
  },
  methods: {
    savePlayer: function() {

      this.formHasErrors = false

      Object.keys(this.form).forEach(f => {
        if (!this.form[f]) this.formHasErrors = true
      });

      if (!this.formHasErrors) {
        Api.addPlayer(this.first_name, this.last_name);
        this.show = close;
      }
    }


  }
}


