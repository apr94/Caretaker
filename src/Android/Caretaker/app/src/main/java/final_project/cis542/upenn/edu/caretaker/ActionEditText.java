package final_project.cis542.upenn.edu.caretaker;


/**
 * Created by Aravind on 11/9/14.
 * This is used to add more functionality to the input edittext
 * such as make it multiline and disable newline.
 */



        import android.content.Context;
        import android.util.AttributeSet;
        import android.view.inputmethod.EditorInfo;
        import android.view.inputmethod.InputConnection;
        import android.widget.EditText;

// An EditText that lets you use actions ("Done", "Go", etc.) on multi-line edits.
public class ActionEditText extends EditText
{
    // The first three methods call the parent methods.
    public ActionEditText(Context context)
    {
        super(context);
    }

    public ActionEditText(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }

    public ActionEditText(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);
    }

    @Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs)
    {
        // make sure the correct flag to disable enter is set.
        InputConnection conn = super.onCreateInputConnection(outAttrs);
        outAttrs.imeOptions &= ~EditorInfo.IME_FLAG_NO_ENTER_ACTION;
        return conn;
    }
}