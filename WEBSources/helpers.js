//Compare two fields in a form - submit if the same

function CompareFields(aForm) {
    if (aForm.field1.value != aForm.field2.value) {
        alert("Inputs don't match!")
        return false;

    } else { 
        return true; 
    }
}